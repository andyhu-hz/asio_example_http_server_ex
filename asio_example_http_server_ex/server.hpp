
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

	/// The top-level class of the HTTP server.
	class server
		: private boost::noncopyable
	{
	public:
		/// Construct the server to listen on the specified TCP address and port, and
		/// serve up files from the given directory.
		explicit server(const std::string& address, const std::string& port, std::size_t io_service_pool_size);

		/// Run the server's io_service loop.
		void run();

		void request_handler(request_handler_t handler)
		{
			request_handler_ = std::move(handler);
		}
	private:
		/// Initiate an asynchronous accept operation.
		void start_accept();

		/// Handle completion of an asynchronous accept operation.
		void handle_accept(const boost::system::error_code& e);

		/// Handle a request to stop the server.
		void handle_stop();

		/// The pool of io_service objects used to perform asynchronous operations.
		io_service_pool io_service_pool_;

		/// The signal_set is used to register for process termination notifications.
		boost::asio::signal_set signals_;

		/// Acceptor used to listen for incoming connections.
		boost::asio::ip::tcp::acceptor acceptor_;

		connection_ptr new_connection_;

		request_handler_t request_handler_;
	};

}