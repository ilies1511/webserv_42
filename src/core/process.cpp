#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

void	Connection::handle_delete(void)
{}

void	Connection::handle_post(void)
{}

void	Connection::handle_get(void)
{
	/*
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
			do_normalFile //vielleicht damit anfangen
				if () {
					_fdFile = open();
					if (_fdFile)
						generate_error_response
				}
				check if file here (open)
			READ_FILE STATE
			return ;
		}
	*/
}

void	Connection::methode_handler(void)
{
	if (this->request._method == "GET")
	{
		handle_get();
	}
	else if (this->request._method == "POST")
	{
		handle_post();
	}
	else if (this->request._method == "DELETE")
	{
		handle_delete();
	}
	else {
		//TODO: handle error for wrong methode
	}
}

void	Connection::entry_process(void)
{
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
						do_normalFile //vielleicht damit anfangen
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
	this->methode_handler();
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

	if (request.readFile && !(this->_current_response.FileData))
	{
		_current_response.process_request(this->request);
		std::cout << "PROCESS: In Conditional Block \n";
		_state = State::READ_FILE; // Und von hier aus geht man ja wieder zuruecl zu Execution State

		//TODO: checken ob ich im Code hier weiterkomme
		printer::Header("PROCESS - Post _state = State::READ_FILE Assigment ");
		//TODO: 17.03.25 - Extern File Handling in function --> generate_response_body()
		if (prepare_fdFile()) {
			_state = State::READ_FILE; // Und von hier aus geht man ja wieder zuruecl zu Execution State
			return ;
		}
		else if (prepare_ErrorFile()) {
			printer::debug_putstr("In open index.html failed", __FILE__, __FUNCTION__, __LINE__);
			_state = State::READ_FILE; //Nach wie READ_FILE State allerings mit anderem Status Code, sprich ERROR - RESPONse
			//TODO: ERROR HANDLING --> Standard Error Msg
		}
		else {
			//TODO: Generate HardCoded Response Body
			generate_internal_server_error_response();
			assemble_response();
			_state = State::SEND; //Nach wie READ_FILE State allerings mit anderem Status Code, sprich ERROR - RESPONse
			return ;
		}
		// Extern File -- END
		printer::Header("PROCESS - Post _server.add_to_pollfds_prefilled(new_fd)");

		/* TODO: bevor man direkt in SEND geht, nochmal final Checken, ob
				response ready zum senden ist. Auch z.B mit
				this->ready2send(_current_response)
						bool ready2send(Response &response)

			if (_current_response.ready2send) // oder if (this->ready2send(_current_response))
			{
				_state = State::SEND;
			}
		*/
			// Und von hier aus geht man ja wieder zuruecl zu Execution State
		return;
	}
	else
	{
		/*
			TODO: Extra Check before going to SEND State
			if (_current_response.ready2send) // oder if (this->ready2send(_current_response))
			{
				_state = State::SEND;
			}
		*/

		//TODO: 17.03.2025 _current_response ==> eine Richtig Formartierte Response zusammenbauen --> CHROME TEST

		printer::Header("PROCESS - In else Block pre SEND State");
		assemble_response();
		/* TODO: bevor man direkt in SEND geht, nochmal final Checken, ob
				response ready zum senden ist. Auch z.B mit
				this->ready2send(_current_response)
						bool ready2send(Response &response)
			// if (_current_response.ready2send) // oder if (this->ready2send(_current_response))
			// {
			// 	_state = State::SEND;
			// }
		*/
		_state = State::SEND;

		/*
			Hier baut man den Response String zusammen. Man liest nur den File
			sondern
			z.B innerhalb einer process() Funktionen oder Execection
				siehe Response.cpp
		*/
		//TODO: 14.03

		// response->some_data = request->other_execution;
		// _current_response.response_inzemaking = request.execute_and_generate_response();


		/*
			Ziel:
								std::string http_response = "HTTP/1.1 200 OK\r\n"
											"Content-Type: " +
											content_type + "\r\n"
															"Content-Length: " +
											std::to_string(_current_response.body.size()) + "\r\n"
																							"\r\n" +
											_current_response.body;

				_OutputBuffer._buffer.assign(http_response.begin(), http_response.end());
		*/
	}
}
