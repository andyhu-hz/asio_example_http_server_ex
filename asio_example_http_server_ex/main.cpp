
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "server.hpp"

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 4)
		{
			std::cerr << "Usage: http_server <address> <port> <threads>\n";
			std::cerr << "  For IPv4, try:\n";
			std::cerr << "    receiver 0.0.0.0 80 1\n";
			std::cerr << "  For IPv6, try:\n";
			std::cerr << "    receiver 0::0 80 1\n";
			return 1;
		}

		std::size_t num_threads = boost::lexical_cast<std::size_t>(argv[3]);
		timax::server s(argv[1], argv[2], num_threads);
		s.request_handler([](const timax::request& req, timax::reply& rep)
		{
			rep.status = timax::reply::ok;
			rep.headers.emplace_back(timax::header{ "Content-Length", "11" });
			rep.content = "Hello World";
		});

		s.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
