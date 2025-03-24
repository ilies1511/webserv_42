#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

void	Connection::handle_delete(void)
{}

void	Connection::handle_post(void)
{}

void	Connection::methode_handler(void)
{
	if (this->request.method == "GET")
	{
		std::cout << coloring("in methode_handler GET", PURPLE);
		handle_get();
	}
	else if (this->request.method == "POST")
	{
		std::cout << coloring("in methode_handler", PURPLE);
		handle_post();
	}
	else if (this->request.method == "DELETE")
	{
		handle_delete();
	}
	else {
		assert(0);
		//TODO: handle error for wrong methode
	}
}
