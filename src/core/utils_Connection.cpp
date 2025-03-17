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
	_current_response.http_version = "HTTP/1.1";
	_current_response.status_code = "200";
	_current_response.headers["Connection"] = "close";
	_current_response.headers["Content-Type"] = "text/html";
	_current_response.headers["Content-Length"] = \
		std::to_string(this->_current_response.file_data.size());

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
		  <h1>Internal Server Error</h1>
		  <p>The server was unable to complete your request. Please try again later.</p>
		</body>
		</html>)";
	_current_response.headers["Content-Length"] = \
		std::to_string(_current_response.file_data.size());
	return ;
}
//Utils -- END


