#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
// #include "StaticFileHandler.hpp"
#include <map>
#include <filesystem>
#include <sstream>

//TODO: just a placeholder for now
bool Connection::is_cgi(std::string &path) {
	if (request.status_code.has_value())
	{
		return (false);
	}
	if (path.length() > 3
		&& path.find_last_of(".py") == (size_t)(path.end() - path.begin() - 3)
		&& (request.method == "GET" || request.method == "POST"))
	{
		return (true);
	}
	return (false);
}

void	Connection::entry_cgi(void)
{
	/*
	if (run_cgi is finished)
	{
		if (!(cgi had error))
		{
			_state = _next_state;
		}
		else
		{
			set_full_status_code(cgi_status_code);
			return ;
		}
	}
	*/
}
