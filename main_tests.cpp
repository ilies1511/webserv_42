#include "webserv.hpp"

int main(int argc, char* argv[])
{
	Log	log;

	(void)argc;
	(void)argv;
	if(argc == 1)
	{
		log.complain("INFO", "Gebrauch: ihost Rechnername", __FILE__, __FUNCTION__, __LINE__);
	}
	else
		play(argv);
}
