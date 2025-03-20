#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>
#include <filesystem>

void	Connection::handle_delete(void)
{}

void	Connection::handle_post(void)
{}

void	Connection::handle_get(void)
{
	std::cout << coloring("In handle_get", PURPLE);
	if (std::filesystem::is_directory(this->_system_path)) {
		assert(0);
		// if (indexfile_here)
		// {
		// 	setpath2indexfile();
		// }
		// else if (autoindex_here) {

		// }
		// else {
		// 	error (404);
		// }
		// here: execute_file()
	} else {
		// 1. check if file --> if not, respone 404
		if (!std::filesystem::exists(_system_path))
		{
			prepare_ErrorFile(); //404
			return ;
		}
		// if (cgi)
		if (0)
		{
			//do_cgi
		}
		else
		{
			//do_normalFile
			/*
				TODO:	funcs sollen als params _state und _next_state haben und im
				function-body die jeweiligen Attribute darauf setzten
				davor if (request.readFile && !(this->_current_response.FileData))
			*/
			prepare_fdFile();
			/*
				prepare_fdFile() {
					do_normalFile //vielleicht damit anfangen
					if () {
						_fdFile = open();
						if (_fdFile)
						generate_error_response
						}
						check if file here (open)
						READ_FILE STATE
				}
			*/
		}
	}
}

void	Connection::methode_handler(void)
{
	if (this->request._method == "GET")
	{
		std::cout << coloring("in methode_handler GET", PURPLE);
		handle_get();
	}
	else if (this->request._method == "POST")
	{
		std::cout << coloring("in methode_handler", PURPLE);
		handle_post();
	}
	else if (this->request._method == "DELETE")
	{
		handle_delete();
	}
	else {
		assert(0);
		//TODO: handle error for wrong methode
	}
}

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
	*/

	/*
		PART 2:
			methode_handler()
	*/
	methode_handler();
	return ;
}











void	Connection::connection_process(void)
{
	std::cout << "Straight outta RECV\n";

	/*
		Dieser Block verstehe ich noch nicht gut genug
			- Alles von CGI muss in Pipe geschrieben und dann auch davon gelesen werden
			- GET-request muss FILE lesen (angegeben durch "uri")
			- Status Code muss man von FILE lesen (z.b 404.html)

	*/
	/*
		// Der Process muss auf Grundlage des fertigen Requests, zu
		ermittlern ob eine File geoffnet werden muss
		Logik fuer Faehigkeit, File zu lesen, in Buffer zu speichern, Body

		Response String Stueck fuer Stueck zusammenbauen und dabei Body lesen
		und dann Resonse generieren  --> siehe void Response::finish_up(void)
		Logik eigenen Buffer speichern.

		Z.B:

		response_str = "HTTP/1.1 200 OK\r\n" --> Header
				"Content-Type: text/plain\r\n"
				"Content-Length: 13\r\n\r\n"
				"Hello World!\n"; --> das was man aus der File liest

		Ueberlegungstatt OutputBuffer, std::string zu verwenden
			--> Gefahr: c_str am Ende nullterminiert, waehrend std::strings nicht

	*/
	// READ_FILE KOENNRE auch eine Sub-State sein.
	printer::debug_putstr("In Process State", __FILE__, __FUNCTION__, __LINE__);

	entry_process();
	return ;
	/*
		0. check if request was invalid --> parser sets status-code xy : read file, send to clieant (assemble_resonse)
		1. Allgemeine Schritte:
			- Path Variable zusammenbauen --> Config + Request = FilePath
			- if location is a redirect
				--> just send 30x + specific Header
			- Verifizierung ob Methode fuer directory valide, if inavlid assemble_response with appropriate response
		2. Method Handler:
			if (request._method  == GET)
			{
				handle_get();
			}
			- GET-Handler: handle_get();
				if (dir)
					if (indexfile_here)
					{
						setpath2indexfile();
					}
					else if (autoindex_here) {

					}
					else {
						error (404);
					}
				here: execute_file()
				if (cgi) {
					do_cgi
				}
				else {
					do_normalFile //vielleicht damit anfangen --> 	if (request.readFile && !(this->_current_response.FileData))
						if () {
							_fdFile = open();
							if (_fdFile)
								generate_error_response
						}
						check if file here (open)
					READ_FILE STATE
					return ;
				}

			- POST-Handler: handle_POST();

	*/

	//_current_response.process_request(this->request);

	//hier davor  --> get_file();
}
