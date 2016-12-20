
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
			//std::cout << req.body() << std::endl;
			if (req.path() == "/")
			{
				rep.add_header("Content-Type", "text/plain");
				rep.response_text("Hello World");
			}
			else if (req.path() == "/chunked")
			{
				rep.add_header("Content-Type", "text/plain");
				auto chunked_num = std::make_shared<int>(0);
				rep.response_by_generator([chunked_num]
				{
					if (*chunked_num == 10)
					{
						*chunked_num = 0;
						return std::string{};
					}

					++*chunked_num;
					return "Hello " + boost::lexical_cast<std::string>(*chunked_num) + "\r\n";
				});
			}
			else if (req.path() == "/file")
			{
				if (!rep.response_file("D:\\WinHex\\1"))
				{
					rep = timax::reply::stock_reply(timax::reply::not_found);
				}
			}
		});

		s.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
