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
	//0:
	if (request.status_code.has_value())
	{
		set_full_status_code((size_t)*request.status_code);
	}
	/*
	TODO: PART 1
		0. check if request was invalid --> parser sets status-code xy : read file, send to clieant (assemble_resonse)
		1. Allgemeine Schritte:
			- location auswaehlen/matching
			- if location is a redirect
				--> just send 30x + specific Header
			- Path Variable zusammenbauen --> Config + Request = FilePath

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
	if (is_cgi(request.uri->path)) {
		this->_state = State::CGI;
		this->_next_state = State::SEND;
		return ;
	}
	methode_handler();
	return ;
}
