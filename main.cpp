#include "webserv.hpp"

int	main(void)
{
	Log	log("webserv");

	log.complain("INFO", "Willkommen", __FILE__, __FUNCTION__, __LINE__);
	printer::Header("main");
	std::cout << "ALO aus main.cpp\n";
	printer::Header("dummy func Call");
	dummy_function();
	printer::Header("src func Call");
	src_function();
	return (0);
}
