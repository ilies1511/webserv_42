#include "Core.hpp"
#include <atomic>

std::atomic<bool> running(true);

int main(void)
{
	signal(SIGINT, sig_handler);
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
	std::cout << "\nReturn from main" << std::endl;
	return (0);
}
