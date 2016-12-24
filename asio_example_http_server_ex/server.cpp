
#include "server.hpp"

#include <boost/bind.hpp>

#include <iostream>

namespace timax
{
	server::server(std::size_t io_service_pool_size)
		: io_service_pool_(io_service_pool_size),
		acceptor_(io_service_pool_.get_io_service()),
		new_connection_()
	{
	}

	void server::run()
	{
		io_service_pool_.run();
	}

	void server::start_accept()
	{
		new_connection_.reset(new connection(io_service_pool_.get_io_service(), request_handler_));
		acceptor_.async_accept(new_connection_->socket(),
			boost::bind(&server::handle_accept, this,
				boost::asio::placeholders::error));
	}

	void server::handle_accept(const boost::system::error_code& e)
	{
		if (!e)
		{
			new_connection_->start();
		}
		else
		{
			std::cout << "server::handle_accept: " << e.message() << std::endl;
		}

		start_accept();
	}

	void server::handle_stop()
	{
		io_service_pool_.stop();
	}

}
