
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

		s.listen(argv[1], argv[2])
			.listen("0.0.0.0", "8089",
R"_(-----BEGIN RSA PRIVATE KEY-----
MIICWwIBAAKBgQDBx36qaiKo9cgOuvfs4SfPaD+uesuGXwfUmCa1tt8O+3Cv71k0
lMdzuQDds9GmcHQpl1i+wxkCdCikyfOZ16ODefJfUYbD1ubq0p9OCnvGZd3GIgO5
GyTLp/ZjOE0VjPWUaJjxWG8TBM18liiTV7vf/R6Ihn7qeBRTAFfOD8XfswIDAQAB
AoGAPYQcVPMun56u7OYz8JIltjUtCjHXTIf8j2Ph3gv40ePGiUOdeRDGJJmVicwd
AQ5wAggW1zBOTzYa9P1bhPos4PBrG9abNlENTpaYv/YgmaNVzdG3soccVz0vZkNZ
q8rS8eNKcatLhy1Hmb/Tb0SgUnygFP8L0YGr0oUgfrQKKLkCQQD/MKDnje0Q2Qb4
+QG1tsSF6hmkT6z2PB9sgzY2Ee6GISfQAhUhRb0t52JDNKy0PRrMiFQH/CN6ayj0
COrJZo4FAkEAwmT2g8ZBUS0ckFvC80WxcIbsMtKY2eadFCDgUP6WN+3M1BdF3fyT
wl1vDJeNgJ3G3SdU7YnraCd7eBjyKDPsVwJANMnlbyD/2k1a0OQJw4JEUwNjPIcy
21kCkxrteu/orJxtNO1JW7Gmebl0rTxXQY2mXosQ/vnciPVj6l9x2im51QJAKtcr
SLEpdeVWNS4z0X8bcqwZY73rt6HOHkB8jvz8SAkdKeSTHi/qtFv4/DJuGdkrmVNw
sQTDH2oMGNSOkXOX8wJAfTJ05Fb1QWKIbty9xkk/KmuaBZGwLoN4Jk7qniud7Q3Q
X1A4BcVVf56Phkta4sCiv7+KabyHqQzi5yfSJYHZ6w==
-----END RSA PRIVATE KEY-----)_",
R"_(-----BEGIN CERTIFICATE-----
MIICATCCAWoCCQC3AxBXOPLZIDANBgkqhkiG9w0BAQsFADBFMQswCQYDVQQGEwJB
VTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50ZXJuZXQgV2lkZ2l0
cyBQdHkgTHRkMB4XDTE1MDgxMDA5MDYzNFoXDTI1MDgwNzA5MDYzNFowRTELMAkG
A1UEBhMCQVUxEzARBgNVBAgMClNvbWUtU3RhdGUxITAfBgNVBAoMGEludGVybmV0
IFdpZGdpdHMgUHR5IEx0ZDCBnzANBgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAwcd+
qmoiqPXIDrr37OEnz2g/rnrLhl8H1JgmtbbfDvtwr+9ZNJTHc7kA3bPRpnB0KZdY
vsMZAnQopMnzmdejg3nyX1GGw9bm6tKfTgp7xmXdxiIDuRsky6f2YzhNFYz1lGiY
8VhvEwTNfJYok1e73/0eiIZ+6ngUUwBXzg/F37MCAwEAATANBgkqhkiG9w0BAQsF
AAOBgQCxBSzoh77Urlzkl/wCR7DbVbyc6m4HtTiP0WLLMNZMlIWbFnN0FBbt1SBM
z3HPfiu+jeD2gwq/batQNzcTHSnXljs/eMTo95auy0E060tr+cUziJZL6eyRGmN5
n0Zb1uvoibww0yRw4Ue+277AVX/nSUkA9eMDg465zR6XCrnLqA==
-----END CERTIFICATE-----
)_", false)
			.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "exception: " << e.what() << "\n";
	}

	return 0;
}
