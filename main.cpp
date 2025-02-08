#include "webserv.hpp"

int	main(void)
{
	printer::Header("main");
	std::cout << "ALO aus main.cpp\n";
	printer::Header("dummy func Call");
	dummy_function();
	printer::Header("src func Call");
	src_function();
	return (0);
}
