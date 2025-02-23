#include "webserv.hpp"
#include "Server.hpp"

int main(void)
{
	Log	log("webserv");
	try
	{
		Server alo((char *)"9034");
		alo.poll_loop(); //MAIN LOOP
	}
	catch(const std::exception& e)
	{
		printer::LogException(e, __FILE__, __FUNCTION__, __LINE__);
	}
	return (0);
}
