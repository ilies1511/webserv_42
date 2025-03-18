#include "webserv.hpp"

void printServerConfig(std::vector<serverConfig>& servers) {
	for (auto& instance : servers) {
		instance.printData();
	}
}

int	main(void)
{

	std::vector<TOKEN> tokenList;
	getToken("configFiles/default.conf", tokenList);
	// printToken(tokenList);

	std::vector<serverConfig> servers;
	try {
		Server alo("9034");
		alo.poll_loop(); //MAIN LOOP
		servers = parsing(tokenList);
	} catch (const std::exception& e) {
		std::cout << "Error: " << e.what() << std::endl;
		return 1;
	}
	printServerConfig(servers);

	printer::Header("main");
	std::cout << "ALO aus main.cpp\n";
	printer::Header("dummy func Call");
	dummy_function();
	printer::Header("src func Call");
	src_function();
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
	// catch(const std::exception& e)
	// {
	// 	printer::LogException(e, __FILE__, __FUNCTION__, __LINE__);
	// }
	return (0);
}
