#include "webserv.hpp"

int	main(void)
{

	std::vector<TOKEN> tokenList;

	getToken("configFiles/default.conf", tokenList);
	printToken(tokenList);

	//setting up 2 basic servers
	std::vector<serverConfig> servers;
	parsing(tokenList);
	// servers.emplace_back(80);
	// servers.emplace_back(8080);

	
	printer::Header("main");
	std::cout << "ALO aus main.cpp\n";
	printer::Header("dummy func Call");
	dummy_function();
	printer::Header("src func Call");
	src_function();
	return (0);
}
