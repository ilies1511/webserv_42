#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

//Utils -- BEGIN
pollfd*	Connection::getPollFdElementRoot(int &fd)
{
	for (auto& p : _server._pollfds) {
		if (p.fd == fd) {
			return &p;
		}
	}
	return (nullptr);
}

bool	Connection::check_revent(int &fd, short rrevent)
{
	pollfd	*pfd = getPollFdElementRoot(fd);

	return (pfd && pfd->revents & rrevent);

	// if (pfd && pfd->revents & rrevent) {
	// 	return (true);
	// }
	// return (false);
}

void	Connection::ft_closeNcleanRoot(int &fd)
{
	// // 1. Schließe den Socket
	// if (fd != -1)
	// {
	// 	std::cout << "Closing fd: " << fd << "\n";
	// 	close(fd);
	// }
	// _server.del_from_map(fd);
	// // 3. Entferne aus pollfds
	// _server.del_from_pollfds(fd);
	// // 4. Setze FD auf ungültigen Wert
	// // fd = -1;
	_server.ft_closeNclean(fd);
}

void Connection::generate_error_response(Response &response)
{
	const std::map<std::string, std::string> status_texts =
		{
			{"400", "Bad Request"},
			{"403", "Forbidden"},
			{"404", "Not Found"},
			{"500", "Internal Server Error"}};
	response.body = "HTTP/1.1 " + response.status_code + " " +
					status_texts.at(response.status_code) + "\r\n"
															"Content-Type: text/html\r\n"
															"Content-Length: " +
					std::to_string(response.body.size()) + "\r\n"
														   "\r\n"
														   "<html><body><h1>" +
					response.status_code + " - " +
					status_texts.at(response.status_code) + "</h1></body></html>";
}

void	Connection::print_request_data(Request &request)
{
	std::cout << " Method: " \
					<< request._method << " URI:" \
					<< request._uri \
					<< " Version: " << request._version \
					<< " Finished: " << request.is_finished \
					<< "\n";
}

bool	Connection::prepare_fdFile(void)
{
	_fdFile = open("html/index.html", O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		printer::debug_putstr("In open index.html failed", __FILE__, __FUNCTION__, __LINE__);
		// exit (1);
		//TODO: Error Handling
		return (false);
	}
	else {
		printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
		struct pollfd	new_fd;
		new_fd.fd = _fdFile;
		new_fd.events = POLLIN;
		new_fd.revents = 0;
		this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()
		return (true);
	}

	//Davor war folgender Block im READ_FILE State
	// _fdFile = open("html/index.html", O_RDONLY | O_NONBLOCK);
	// if (_fdFile < 0)
	// {
	// 	printer::debug_putstr("In open index.html failed", __FILE__, __FUNCTION__, __LINE__);
	// 	exit (1);
	// 	//Error Handling
	// }
	// else {
	// 	printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
	// 	struct pollfd	new_fd;
	// 	new_fd.fd = _fdFile;
	// 	new_fd.events = POLLIN;
	// 	new_fd.revents = 0;
	// 	this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()
	// }

}

bool	Connection::prepare_ErrorFile(void)
{
	_fdFile = open("html/error.html", O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		printer::debug_putstr("In open error.html failed", __FILE__, __FUNCTION__, __LINE__);
		return (false);
	}
	printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
	struct pollfd	new_fd;
	new_fd.fd = _fdFile;
	new_fd.events = POLLIN;
	new_fd.revents = 0;
	this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()

	_current_response.status_code = "404";
	_current_response.headers["Connection"] = "close";
	return (true);
}

// void	Connection::assemble_response2(Response &response)
// {
// 	(void)response;
// 	// response.http_version = "HTTP/1.1";
// 	// response.status_code = "200";
// 	// response.reason_phrase = "OK";

// 	// // Header-Felder setzen
// 	// response.headers["Content-Type"] = "text/html";
// 	// // Hier wäre 137 die Gesamtzahl der Bytes im Body – in der Praxis berechnest du das dynamisch:
// 	// response.headers["Content-Length"] = "137";
// 	// response.headers["Connection"] = "close";

// 	// // Den Body der Antwort definieren
// 	// response.body = "<html>\r\n"
// 	//             "  <head><title>Example</title></head>\r\n"
// 	//             "  <body><h1>Hello, World!</h1></body>\r\n"
// 	            // "</html>";
// }

void Connection::assemble_response2(void)
{
	const std::map<std::string, std::string> status_texts =
		{
			{"200", "OK"},
			{"400", "Bad Request"},
			{"403", "Forbidden"},
			{"404", "Not Found"},
			{"500", "Internal Server Error"}};
	_current_response.response_inzemaking =
								_current_response.http_version + _current_response.status_code + " " +
								status_texts.at(_current_response.status_code) + "\r\n"
								"Content-Type: text/html\r\n"
								"Content-Length: " +
								std::to_string(_current_response.body.size()) + "\r\n"
								"\r\n"
								"<html><body><h1>" +
								_current_response.status_code + " - " +
								status_texts.at(_current_response.status_code) + "</h1></body></html>";
}

void Connection::assemble_response()
{
	std::string reason = _current_response.reason_phrase;

	//Add hardcoded Headers
	if (_current_response.status_code != "500") // condition nur temporaer da um zu 500 Case zu testen
	{
		_current_response.http_version = "HTTP/1.1";
		_current_response.status_code = "200";
		_current_response.headers["Connection"] = "close";
		_current_response.headers["Content-Type"] = "text/html";
		_current_response.headers["Content-Length"] = \
		std::to_string(this->_current_response.file_data.size());
	}

	if (reason.empty()) {
		const std::map<std::string, std::string> default_status_texts = {
			{"200", "OK"},
			{"400", "Bad Request"},
			{"403", "Forbidden"},
			{"404", "Not Found"},
			{"500", "Internal Server Error"}
		};
		auto it = default_status_texts.find(_current_response.status_code);
		if (it != default_status_texts.end()) {
			reason = it->second;
		}
	}

	// Assemble status line (HTTP-Version, Statuscode, Reason-Phrase)
	std::string assembled_response =	_current_response.http_version + " " +
										_current_response.status_code + " " +
										reason + "\r\n";

	// Alle Header durchlaufen und hinzufügen
	for (const auto &header : _current_response.headers) {
		assembled_response += header.first + ": " + header.second + "\r\n";
	}

	//Add line
	assembled_response += "\r\n";

	//Append file_data
	assembled_response += _current_response.file_data;

	// FF - FInohed Fusion
	_current_response.response_inzemaking = assembled_response;
}

void	Connection::generate_internal_server_error_response(void)
{
	_current_response.http_version = "HTTP/1.1";
	_current_response.status_code = "500";
	_current_response.headers["Content-Type"] = "text/html";
	_current_response.headers["Connection"] = "close";
	_current_response.file_data = \
		R"(<!doctype html>
		<html lang="en">
		<head>
		  <title>500 Internal Server Error</title>
		</head>
		<body>
		  <h1>ALOOOOOOOOO</h1>
		  <p>The server was unable to complete your request. Please try again later.</p>
		</body>
		</html>)";
	_current_response.headers["Content-Length"] = \
		std::to_string(_current_response.file_data.size());
	return ;
}

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

void	Connection::read_file(void)
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
}

void	Connection::write_file(void)
{
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
}

void	Connection::send_data(void)
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
//Utils -- END


