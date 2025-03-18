#include "webserv.hpp"

void printServerConfig(std::vector<serverConfig>& servers) {
	for (auto& instance : servers) {
		instance.printData();
	}
}

int	main(void)
{
	Log	log("webserv");

	log.complain("INFO", "Willkommen, connect to server via telnet localhost 9034 in an other terminal", __FILE__, __FUNCTION__, __LINE__);
	try
	{
		Server alo("9034");
		alo.poll_loop(); //MAIN LOOP
	}
	catch(const std::exception& e)
	{
		printer::LogException(e, __FILE__, __FUNCTION__, __LINE__);
	}
	// try{
	// 	servers = parsing(tokenList);
	// } catch (const std::exception& e) {
	// 	std::cout << "Error: " << e.what() << std::endl;
	// 	return 1;
	// }
	// printServerConfig(servers);
	return (0);
}
