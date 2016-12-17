
#pragma once

#include "reply.hpp"
#include "request.hpp"

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace timax
{
	using request_handler_t = boost::function<void(const request& req, reply& rep)>;

	class connection
		: public boost::enable_shared_from_this<connection>,
		private boost::noncopyable
	{
	public:
		explicit connection(boost::asio::io_service& io_service, request_handler_t& handler);

		boost::asio::ip::tcp::socket& socket();

		void start();

		void close();

	private:

		void reset_timer(int seconds = 60);

		void do_read();

		void do_read_body();

		void do_write();

		void do_request();

		void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);

		void handle_write(const boost::system::error_code& e);

		bool check_keep_alive();
	private:
		boost::asio::ip::tcp::socket socket_;

		request_handler_t& request_handler_;

		std::vector<char> buffer_;
		std::size_t nread_;


		request request_;

		reply reply_;

		bool keep_alive_ = false;

		boost::asio::deadline_timer deadline_;
	};

	using connection_ptr = boost::shared_ptr<connection>;
}
