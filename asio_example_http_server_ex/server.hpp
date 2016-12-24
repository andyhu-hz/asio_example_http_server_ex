
#pragma once

#include "connection.hpp"
#include "io_service_pool.hpp"

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace timax
{

	class server : private boost::noncopyable
	{
	public:
		explicit server(std::size_t io_service_pool_size);

		server& listen(const std::string& address, const std::string& port)
		{
			boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
			boost::asio::ip::tcp::resolver::query query(address, port);
			boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
			acceptor_.open(endpoint.protocol());
			acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor_.bind(endpoint);
			acceptor_.listen();
			start_accept();
			return *this;
		}

		void run();

		void request_handler(request_handler_t handler)
		{
			request_handler_ = std::move(handler);
		}
	private:
		void start_accept();

		void handle_accept(const boost::system::error_code& e);

		void handle_stop();

		io_service_pool io_service_pool_;

		boost::asio::ip::tcp::acceptor acceptor_;

		connection_ptr new_connection_;

		request_handler_t request_handler_;
	};

}