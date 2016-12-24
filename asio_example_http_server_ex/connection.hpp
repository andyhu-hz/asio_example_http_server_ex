
#pragma once

#include "reply.hpp"
#include "request.hpp"
#include "utils.h"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <vector>

namespace timax
{
	using request_handler_t = boost::function<void(const request& req, reply& rep)>;

	template <typename socket_type>
	class connection
		: public boost::enable_shared_from_this<connection<socket_type>>,
		private boost::noncopyable
	{
	public:
		explicit connection(boost::asio::io_service& io_service, request_handler_t& handler)
			: socket_(io_service), request_handler_(handler),
			buffer_(8192), deadline_(io_service)
		{
		}

		explicit connection(boost::asio::io_service& io_service, request_handler_t& handler, boost::asio::ssl::context& ctx)
			: socket_(io_service, ctx), request_handler_(handler),
			buffer_(8192), deadline_(io_service)
		{
		}

		socket_type&  socket()
		{
			return socket_;
		}

		void start()
		{
			nread_ = 0;
			do_read();
		}

		void close()
		{
			do_close(socket_);
		}

		void reset_timer(int seconds = 60)
		{
			deadline_.expires_from_now(boost::posix_time::seconds(seconds));	//TODO:超时时间改为可配置
			boost::weak_ptr<connection<socket_type>> weak_self = this->shared_from_this();

			deadline_.async_wait([weak_self](boost::system::error_code const& ec)
			{
				auto self = weak_self.lock();

				if (!self || ec)
				{
					return;
				}

				self->close();
			});
		}

	private:
		void do_close(boost::asio::ip::tcp::socket const&)
		{
			boost::system::error_code ec;
			socket_.close(ec);
		}
		void do_close(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const&)
		{
			boost::system::error_code ec;
			socket_.lowest_layer().close(ec);
		}

		void do_read()
		{
			reset_timer();
			if (buffer_.size() - nread_ < 4096)
			{
				buffer_.resize(buffer_.size() + 8192);
			}
			socket_.async_read_some(boost::asio::buffer(buffer_.data() + nread_, buffer_.size() - nread_),
				boost::bind(&connection::handle_read, shared_from_this(),
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}

		void do_read_body()
		{
			reset_timer();
			std::size_t req_len = request_.header_size() + request_.body_len();
			if (buffer_.size()< req_len)
			{
				buffer_.resize(req_len);
			}

			auto self = this->shared_from_this();
			boost::asio::async_read(socket_, boost::asio::buffer(buffer_.data() + nread_, req_len - nread_),
				[self, this](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (ec)
				{
					return;
				}
				/*
				if (length != request_.header_size() + request_.body_len())
				{
				return;
				}
				*/

				do_request();
			});
		}

		void do_write()
		{
			reset_timer();
			std::vector<boost::asio::const_buffer> buffers;
			write_finished_ = reply_.to_buffers(buffers);
			if (buffers.empty())
			{
				handle_write(boost::system::error_code{});
				return;
			}

			boost::asio::async_write(socket_, buffers,
				boost::bind(&connection::handle_write, shared_from_this(),
					boost::asio::placeholders::error));
		}

		void do_request()
		{
			keep_alive_ = check_keep_alive();

			if (request_handler_)
			{
				request_handler_(request_, reply_);
			}
			else
			{
				reply_ = reply::stock_reply(reply::bad_request);
			}

			if (keep_alive_)
			{
				reply_.add_header("Connection", "keep-alive");
			}
			else
			{
				reply_.add_header("Connection", "close");
			}

			do_write();
		}

		void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
		{
			if (e)
			{
				if (e == boost::asio::error::eof)
				{
					shutdown(socket_);
				}
				return;
			}

			size_t last_len = nread_;
			nread_ += bytes_transferred;

			if (nread_ >= 2 * 1024 * 1024)
			{
				// 请求过大,断开链接
				return;
			}

			int ret = request_.parse(buffer_.data(), nread_, last_len);

			if (ret == -1)
			{
				//TODO: 判断是否可以升级到http2
				reply_ = reply::stock_reply(reply::bad_request);
				do_write();
				return;
			}

			if (ret == -2)
			{
				do_read();
				return;
			}


			if (request_.header_size() + request_.body_len() >= 2 * 1024 * 1024)
			{
				return;
			}

			if (request_.body_len() != 0)
			{
				do_read_body();
				return;
			}

			do_request();
		}

		void handle_write(const boost::system::error_code& e)
		{
			if (e)
			{
				return;
			}

			if (write_finished_)
			{
				reply_.reset();

				if (!keep_alive_)
				{
					shutdown(socket_);
					return;
				}

				start();
				return;
			}
			do_write();
		}

		bool check_keep_alive()
		{
			auto conn_hdr = request_.get_header("connection", 10);
			if (request_.is_http1_1())
			{
				// HTTP1.1
				//头部中没有包含connection字段
				//或者头部中包含了connection字段但是值不为close
				//这种情况是长连接
				//keep_alive_ = conn_hdr.empty() || !boost::iequals(conn_hdr, "close");
				return conn_hdr.empty() || !iequal(conn_hdr.data(), conn_hdr.size(), "close", 5);
			}
			else
			{
				//HTTP1.0或其他(0.9 or ?)
				//头部包含connection,并且connection字段值为keep-alive
				//这种情况下是长连接
				//keep_alive_ = !conn_hdr.empty() && boost::iequals(conn_hdr, "keep-alive");
				return !conn_hdr.empty() && iequal(conn_hdr.data(), conn_hdr.size(), "keep-alive", 10);
			}
		}

		void shutdown(boost::asio::ip::tcp::socket const&)
		{
			boost::system::error_code ignored_ec;
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
		}
		void shutdown(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> const&)
		{
		}


	private:
		socket_type socket_;

		request_handler_t& request_handler_;

		std::vector<char> buffer_;
		std::size_t nread_;


		request request_;

		bool write_finished_;
		reply reply_;

		bool keep_alive_ = false;

		boost::asio::deadline_timer deadline_;
	};
}
