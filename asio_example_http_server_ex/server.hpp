
#pragma once

#include "io_service_pool.hpp"
#include "connection.hpp"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
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

		server& listen(const std::string& address, const std::string& port);
		server& listen(const std::string& address, const std::string& port,
			const std::string& private_key, const std::string& certificate_chain, bool is_file = true);

		void run();

		void request_handler(request_handler_t handler)
		{
			request_handler_ = std::move(handler);
		}

		void stop();

	private:
		void start_accept(boost::shared_ptr<boost::asio::ip::tcp::acceptor> const& acceptor);
		void start_accept_tls(boost::shared_ptr<boost::asio::ip::tcp::acceptor> const& acceptor);

		void do_listen(boost::shared_ptr<boost::asio::ip::tcp::acceptor>& acceptor,
			const std::string& address, const std::string& port);

		io_service_pool io_service_pool_;

		boost::asio::ssl::context tls_ctx_;;

		request_handler_t request_handler_;
	};

}