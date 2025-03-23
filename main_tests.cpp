#include "webserv.hpp"
#include "Server.hpp"

int main(void)
{

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

	// Log	log("webserv");
	// try
	// {
	// 	Server alo((char *)"9034");
	// 	alo.poll_loop(); //MAIN LOOP
	// }
	// catch(const std::exception& e)
	// {
	// 	printer::LogException(e, __FILE__, __FUNCTION__, __LINE__);
	// }
	return (0);
}
