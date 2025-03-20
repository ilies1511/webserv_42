#include <charconv>

#include "webserv.hpp"

// std::string exampleGetRequest() {
//
//     std::string test = "GET /favicon.ico HTTP/1.1\r\nHost: localhost\r\nConnection: keep-alive\r\nsec-ch-ua: \"Chromium\";v=\"128\", \"Not;A=Brand\";v=\"24\", \"Google Chrome\";v=\"128\"\r\nsec-ch-ua-mobile: ?0\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/128.0.0.0 Safari/537.36\r\nsec-ch-ua-platform: \"macOS\"\r\nAccept: image/avif,image/webp,image/apng,image/svg+xml,image/*,*/*;q=0.8\r\nSec-Fetch-Site: same-origin\r\nSec-Fetch-Mode: no-cors\r\nSec-Fetch-Dest: image\r\nReferer: http://localhost/\r\nAccept-Encoding: gzip, deflate, br, zstd\r\nAccept-Language: en-GB,en-US;q=0.9,en;q=0.8\r\n\r\n";
// 	return test;
// }

int	main(void)
{

	std::vector<TOKEN> tokenList;
	getToken("configFiles/default.conf", tokenList);
	// printToken(tokenList);

	std::vector<serverConfig> servers;
	try {
		// servers = parsing(tokenList);
		Server alo("9034");
		alo.poll_loop(); //MAIN LOOP
	} catch (const std::exception& e) {
		std::cout << coloring("Error: " + std::string(e.what()), RED) << std::endl;
		return 1;
	}
	// const std::string request = exampleGetRequest();
	// requestParser request_parser(request, servers);
	// request_parser.parseMessage();


	// Log	log("webserv");

	// log.complain("INFO", "Willkommen", __FILE__, __FUNCTION__, __LINE__);
	// printer::Header("main");
	// std::cout << "ALO aus main.cpp\n";
	// printer::Header("dummy func Call");
	// dummy_function();
	// printer::Header("src func Call");
	// src_function();
	// return (0);

	// Log	log("webserv");

	// log.complain("INFO", "Willkommen, connect to server via telnet localhost 9034 in an other terminal", __FILE__, __FUNCTION__, __LINE__);
	// try
	// {
	// 	Server alo("9034");
	// 	alo.poll_loop(); //MAIN LOOP
	// }
	// printServerConfig(servers);
	return (0);
}
