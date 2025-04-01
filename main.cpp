#include "Core.hpp"

int main(void)
{
	Log	log("webserv");
	try
	{
		std::cout << coloring("ALO\n", LIGHT_RED);
		Core core;
		core.poll_loop(); //MAIN LOOP
	}
	catch(const std::exception& e)
	{
		printer::LogException(e, __FILE__, __FUNCTION__, __LINE__);
	}
	return (0);
}
