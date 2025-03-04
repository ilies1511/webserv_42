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
	return (0);
}
