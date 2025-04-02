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

/*
std::string a = "hi";
std::string b = "abc";

std::cout << a << " == a\n";
std::cout << b << " == b\n\n";


b = std::move(a);
std::cout << a << " == a\n";
std::cout << b << " == b\n";


char *aa = "hi";

//char *bb = strdup(aa);
char **bb = aa;
aa = NULL;
*/
