#include "webserv.hpp"

int main(int argc, char* argv[])
{
	(void)argc;
	(void)argv;
	if(argc == 1)
	{
		std::cout << "Gebrauch: ihost Rechnername\n";
	}
	else
		play(argv);
}
