#include "webserv.hpp"

int	main(void)
{
	// Log	log("webserv");

	// log.complain("INFO", "Willkommen", __FILE__, __FUNCTION__, __LINE__);
	// printer::Header("main");
	// std::cout << "ALO aus main.cpp\n";
	// printer::Header("dummy func Call");
	// dummy_function();
	// printer::Header("src func Call");
	// src_function();
	// return (0);
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
	return (0);
}
