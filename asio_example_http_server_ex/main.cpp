
#include "server.hpp"
#include "utils.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>

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
		timax::server s(num_threads);
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
			else
			{
				rep = timax::reply_static_file("./static", req);
			}
		});

		s.listen(argv[1], argv[2]).run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
