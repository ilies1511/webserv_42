#include "Connection.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

void	Connection::handle_post(void)
{}

void	Connection::methode_handler(void)
{
	if (this->request.method == "GET")
	{
		P_DEBUGC("Entry methode_handler GET", PURPLE);
		handle_get();
	}
	else if (this->request.method == "POST")
	{
		P_DEBUGC("Entry methode_handler POST", PURPLE);
		handle_post();
	}
	else if (this->request.method == "DELETE")
	{
		P_DEBUGC("Entry methode_handler DELETE", PURPLE);
		handle_delete();
	}
	else {
		assert(0);
		//TODO: handle error for wrong methode
	}
}
