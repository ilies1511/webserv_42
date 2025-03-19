#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

Connection::Connection(int fd, Server &server)
	:	_state{State::RECV},
		_server(server),
		request{},
		_current_response{},
		_fdConnection(fd),
		_fdFile(-1),
		sent_bytes(0),
		finished_sending(false),
		_InputBuffer{},
		_OutputBuffer{}
{
	std::cout << "I'm Connection Constructor: cap of buffer: " << _InputBuffer._buffer.capacity() << "Output Buffer: " << _OutputBuffer._buffer.capacity() << "\n";
}

Connection::~Connection(void) {}

int Connection::getFdConnection(void)
{
	return (_fdConnection);
}

// //Methodes -- BEGIN

void	Connection::execute_layer2(void)
{
	// Prevents going in switch - cases again, if Client already hung up
	if (check_revent(_fdConnection, POLLHUP))
	{
		printf("pollserver: socket %d hung up\n", this->_fdConnection);
		std::cout << coloring("INIT Check: Pre ft_closeNcleanRoot()\n", RED);
		ft_closeNcleanRoot(_fdConnection);
		// _server.ft_closeNclean(_fdConnection);
		return ;
	}

	/*
		State-Machnine -- BEGIN
		Manages the Request->Respone cycle
	*/
	switch (_state)
	{
	case State::RECV:
	{
		if (!check_revent(_fdConnection, POLLIN)) {
			return ;
		}
		//Parsing auch in diesem Schritt, koennte aber auch als eigener State abgehandelt werden
		//Empfehlung: von dem Buffer in einen permanten reinkopieren --> simpler, Problem: _InputBuffer._buffer.capacity
		ssize_t bytes = recv(_fdConnection, _InputBuffer._buffer.data(), \
							_InputBuffer._buffer.capacity() - 1, 0);
		_InputBuffer._buffer[(size_t)bytes] = 0;
		printer::debug_putstr("Post recv", __FILE__, __FUNCTION__, __LINE__);
		if (bytes <= 0) //TODO: 12.03 check if return of 0 is valid
		{
			printer::debug_putstr("Post recv '<= case'", __FILE__, __FUNCTION__, __LINE__);
			if (bytes == 0)
			{
				// Connection closed
				printf("pollserver: socket %d hung up\n", this->_fdConnection);
			}
			else
			{
				printer::debug_putstr("Pre perror Connection", __FILE__, __FUNCTION__, __LINE__);
				perror("recv");
			}
			std::cout << coloring("Pre ft_closeNcleanRoot()\n", RED);//TODO: statt clean hier, erst nach der current loop interation z.B mit clean_after
			ft_closeNcleanRoot(_fdConnection);
			return ;
		}

		std::cout << "Received Data:\n"
					<< std::string(_InputBuffer.data(), (size_t)bytes) << "\n";

		//Stand: 14.03.2025 11:50
		// exit (1);
		// Versuche, Header zu parsen

		/*
			ENTRY-Point Parser --> fills Request Instance (Instance belongs to Connection)
		*/
		parser.entry_parse(this->_InputBuffer, request);
		printer::debug_putstr("In RECV State - Parser finished", __FILE__, __FUNCTION__, __LINE__);
		std::cout << " Method: " \
					<< request._method << " URI:" \
					<< request._uri \
					<< " Version: " << request._version \
					<< " Finished: " << request.is_finished \
					<< "\n";
		// if (parser.parse_header(_InputBuffer, request)) // TODO: parse_header = parser
		// {
		// 	printer::debug_putstr("Pre recv", __FILE__, __FUNCTION__, __LINE__);
		// 	// _state = State::READ_BODY;
		// 	_state = State::READ_FILE;
		// 	// _state = State::SEND;
		// 	return ;
		// }
		// if (strlen(_InputBuffer._buffer.data()) > 10)
		// {
		// 	/*TODO:
		// 	// open and read text.txt with polling the read calls on the text.txt fd
		// 	// set content as data to send to connection fd
		// 	*/
		// 	;
		// }

		/*
			Zusatz-Check um zu ueberpruefen ob die Request Datenstruktur ferig ist
			Indikator: Parser muss Condition setzen ob fertig oder noch in Bearbeitung
		*/
		if (this->request.is_finished)
		{
			printer::debug_putstr("Aloo In RECV State - request finished", __FILE__, __FUNCTION__, __LINE__);
			_state = State::PROCESS;
			return ;
		}
		else {
			std::cout << "Enough, Request finished\n";
			//Staying in the parse state, until request.is_finished == true
			return ;
		}
		break;
	}
	case State::PROCESS:
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

		_current_response.process_request(this->request);

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
		return ;
		break;
	}
	case State::READ_FILE:
	{
		/*
			1. PollValue des File_Fd checken, in diesem Fall auf POLLIN
			2. wenn nicht ready return
			3. Wenn ready: In permanent Buffer reinlesen => der Body der Response
				(separat vom recv und vom Response_string) um damit die
				Response zu generieren, weil man nicht in die Zukfunft schauen kann.
					--> Ziel: content-lenght zu ermittlen und dann Body ran-appenden (aus permanten Body Buffer)

				Alternativ:
				auch mit stat() moelgich
		*/

		// if !FILE_POLLIN: // Wenn file nicht ready zum Poll ist --> checken ob aus dem pollfd struct pollin ready ist
		// {
		// 	return ;
		// }

		/*
			//TODO: Imrove Handling --> File should be opened somewhere else already
			For example in Process, where first it's checked whether File should
			be opened
		*/
		if (!check_revent(_fdFile, POLLIN)) {
			printer::debug_putstr("Event failed", __FILE__, __FUNCTION__, __LINE__);
			return ;
		}
		// response->file_data = read(file_fd);
		/*
			TODO: gelesenen bytes sollen irgendwie in das response.file_data rein
		*/
		// char read_file_buffer[4090];
		printer::Header("In READ_FILE");

		Buffer	read_file_buffer;
		ssize_t bytes_read = read(_fdFile, read_file_buffer._buffer.data(), 4090);

		if (bytes_read <= 0)
		{
			printer::debug_putstr("bytes_read <= 0 Case", __FILE__, __FUNCTION__, __LINE__);
			if (bytes_read == 0) {
				_server.ft_closeNclean(_fdFile);
				std::cout << "Set _fdFile: " << _fdFile << " to -1\n";
				_fdFile = -1;
				// close (_fdFile);
				_state = State::PROCESS;
				_current_response.FileData = true;
				// _state = State::SEND;
				_current_response.file_data.assign(read_file_buffer._buffer.begin(), read_file_buffer._buffer.end());
				return ;
				//File ist fertig gelesen
			}
			else if (bytes_read < 0) {
				_server.ft_closeNclean(_fdFile);
				return ;
				//TODO: Improve & understand propor ERROR behavior when occures --> to fix
			}
		}
		else
		{
			printer::debug_putstr("bytes_read > 0 Case PRE SEND", __FILE__, __FUNCTION__, __LINE__);
			// _state = State::SEND;
			_state = State::PROCESS;
			_current_response.file_data.assign(read_file_buffer._buffer.begin(), read_file_buffer._buffer.end());
			//TODO: improve Case is not fully read --> go Back to process and reread again until end of file
			_current_response.FileData = true;
			_server.ft_closeNclean(_fdFile);
			std::cout << "Set _fdFile: " << _fdFile << " to -1\n";
			_fdFile = -1;
			return ;
		}

		/* TODO: 17.03.25 - Improve Handling, sieh structur file
			// if (finished)
			// {
			// 	close(file_fd);
			// 	// _server.ft_closeNclean(_fdFile); // --> ist das gut, hier den Clean-Up zu machen
			// 	_state = State::PROCESS
			// 	return ;
			// }
		*/

		break;
	}
	/*
		Dafuer fuer CGI, um in Pipes zu schreiben --> CGI spezifisch:

		Kontext: z.B ein POST-Request
		state = State::PROCESS;
		das folgende gehoert in den PROCESS Statw
		0. Uri zu path Umwandlung, Method und cgi Determinierung usw.
		1. CGI skript starten mit fork, dub etc, sprich minishell exec

			pipe(pipe_in);
			pipe(pipe_out);
			->
			fork()
			->
			CHILD:
			dub2(pipe_in[0], STDIN);
				pipe_in ist dafuer da, den Request-Body als STDIN zu mappen
					Das CGI Skript verwendet genau diesen Input um auszufuehren
				pipe_out ist dafuer da, den Output des Skript in STDOUT zu mappn
					 --> output des Skripts statt in STDOUt in pipe_out
					 	 gespeichert, also quasi die ganze HTTP-REesponse ist in
						 Pipe_out gespeichert, was wir dann wieder mittels read in
						 ein Buffer (z.B OutBuffer reinlesen), was wiederrum
						 verwenden wird um gesendet zu werden (Erklaerung zu post execve)


			close(pipe_in[0]);
			close(pipe_in[1]);
			dup2(pipe_out[1], STDOUT);
			close(pipe_out[0]);
			close(pipe_out[1]);
			->
			execve();

			Im Parent : PARENT(webserv):
			close(pipe_in[0]);
			close(pipe_out[1]);
			->
			Kontext: wie gekommen POST-Request und das CGI liest die Daten vom
						stdin, in das davor den Request Body schreiben. --> nach dem forken
			state = State::WRITE;
			das folgende gehoert in den WRITE State
			write(pipe_in[1], request_body, buffer_size);
			if (alles geschrieben)
			{
				close(pipe_in[1])
				state = was auch immer Sinn macht --> State eben dafuer praktisch sich non-konform eine Posiotion zu merken oder hinzuwechseln
			}
			->
			Der Output des ausgefuehrten CGI-Skripts und damit einhergehend dessen
			Output (vom Skript) wird in die pipe_out reingeschrieben.
			In diesem Schritt lesen wir nun aus jener pipe mittels read in den
			response_buffer
			state = State::READ
			das folgende gehoert in den READ:
			read_returnval = read(pipe_out[0], response_buffer, buffer_size);
			if (read_returnval <= 0)
			{
				if (read_returnval == 0)
				{
					close(pipe_out[0])
					fertig, also READ_STATE verlassen und auf PROCESS/EXECUTE
					return
				}
			}
			siehe auch webserv/www/eval/post/upload.py --> Beispiel fuer CGI Skript
			->
			send(socket_fd, response_buffer, buffer_size);
	*/
	case State::WRITE:
	{
		//siehe Kommentar oben
	}
	case State::SEND:
	{
		/*
			0. immer event beim POLLFD immer (POLLIN & POLLOUT) initen, statt ueber enable_pollout() zu gehen
			1. CHECK ob _fdConnection revent auf POLLOUT
			2. wenn nein, return
			3. wenn ja, dann send-Vorgang starten: bytes_send = send(_fdConnection, respone_buffer,  respone_buffer.size(), MSG_DONTWAIT)
				--> Checken ob alles geschickt wurde, und erst wenn bytes_send der response.size() entspricht
					dann Connection closen. Wenn nicht, dann position merken, wieder ueber poll gehen und
					da dann wieder ansaetzen und senden;s
					(siehe Client.cpp) --> send()
			4. Close connection
				- close_later: nach jeder Iter cleanup
		*/

		// if !SOCKET_POLLOUT
		// {

		// 	return
		// }
		// printer::debug_putstr("In PRE send", __FILE__, __FUNCTION__, __LINE__);
		printer::debug_putstr("In PRE send File Content", __FILE__, __FUNCTION__, __LINE__);

		if (!check_revent(_fdConnection, POLLOUT))
		{
			printer::debug_putstr("In alo Case", __FILE__, __FUNCTION__, __LINE__);
			return ;
		}

		_OutputBuffer._buffer.assign(_current_response.response_inzemaking.begin(), _current_response.response_inzemaking.end());
		ssize_t sent = send(this->_fdConnection, this->_OutputBuffer.data(), _OutputBuffer._buffer.size(), 0);
		if (sent > 0)
		{
			this->sent_bytes = sent_bytes + sent;
			printer::debug_putstr("In Body send", __FILE__, __FUNCTION__, __LINE__);
			std::cout << "Sent Response:\n"
					  << std::string(_OutputBuffer.data(), (size_t)sent) << "\n";
			this->_OutputBuffer._buffer.clear();
			// if (sent == (ssize_t)_current_response.response_inzemaking.size()) {
			if (sent_bytes == (ssize_t)_current_response.response_inzemaking.size()) {
				finished_sending = true;
				std::cout << "sent_bytes: " << sent_bytes << ", sent: " << sent << "\n";
				// _state = State::CLOSING; // Oder READ_HEADER für Keep-Alive damit circular ist
				printer::debug_putstr("sent == response_inzemaking.size() 'case'",\
					__FILE__, __FUNCTION__, __LINE__);
				if (this->request.readFile) {
					ft_closeNcleanRoot(_fdFile);
				}
				ft_closeNcleanRoot(this->_fdConnection);
				return ;
			}
		}
		_state = State::SEND;
		// /*
		// 	TODO: 17.03.25
		// 		Partiell send Handling
		// 		Da komme ich nur hier, wenn ich noch was senden muss
		// 		Folgender Block gerade ueberfluessig
		// */
		// printer::debug_putstr("POST in handle_output", __FILE__, __FUNCTION__, __LINE__);
		// /*
		// 	response_buffer ko
		// */
		// // if !finished // Bei langer Response (1GB), viele Calls notwendih
		// // 	return
		// _server.ft_closeNclean(this->_fdFile);
		// _server.ft_closeNclean(this->_fdConnection);
		// printer::debug_putstr("POST in handle_output", __FILE__, __FUNCTION__, __LINE__);
		// return ;
	}
}
	printer::debug_putstr("PRE execute_layer2", __FILE__, __FUNCTION__, __LINE__);
}

// Methodes -- END
