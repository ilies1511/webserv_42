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

void	Connection::entry_process(void)
{
	/*
	TODO: PART 1
		0. check if request was invalid --> parser sets status-code xy : read file, send to clieant (assemble_resonse)
		1. Allgemeine Schritte:
			- Path Variable zusammenbauen --> Config + Request = FilePath
			- if location is a redirect
				--> just send 30x + specific Header
			- Verifizierung ob Meth

		2. Check CGI:
			Conditional Blocks ((is_get || is_post() )&& is_cgi(_sys_path))
			{
				_state = State::CGI; //Kein write mehr notwending, da in CGI Mode gemacht wird
				_nex_state = SEND;
			}
			else {
				do normal cycle
			}
	*/

	/*
		PART 2:
			methode_handler()
	*/

	// request.method = "GET";
	// request.uri = "/index.html";
	// request._version = "HTTP/1.1";
	// request._body = "";
	// // ed_request.is_finished = true;
	// request.is_finished = true;
	// request.readFile = true;
	// // request.filename = "html/index.html";
	// // filled_request._headers = ;
	methode_handler();
	return ;
}
