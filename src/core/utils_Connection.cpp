#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
// #include "Request.hpp"
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

// void	Connection::print_request_data(Request &request)
// {
// 	std::cout << " Method: " \
// 					<< request._method << " URI:" \
// 					<< request._uri \
// 					<< " Version: " << request._version \
// 					<< " Finished: " << request.is_finished \
// 					<< "\n";
// }

void	Connection::prepare_fdFile_param(const std::string status_code)
{
	_current_response.status_code = status_code;

	// _system_path = "errorPages/403.html";
	std::cout << coloring("\n\nin prepare_fdFile - _system_path: " + _system_path + "\n", TURQUOISE);
	_fdFile = open(_system_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		prepare_ErrorFile();
		printer::debug_putstr("In open index.html failed", __FILE__, __FUNCTION__, __LINE__);
		return ;
	}
	else {
		_state = State::READ_FILE;
		_next_state = State::ASSEMBLE;
		printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
		struct pollfd	new_fd;
		new_fd.fd = _fdFile;
		new_fd.events = POLLIN;
		new_fd.revents = 0;
		this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()
		return ;
	}
}

void	Connection::prepare_fdFile(void)
{
	std::cout << coloring("\n\nin prepare_fdFile - _system_path: " + _system_path + "\n", TURQUOISE);
	_fdFile = open(_system_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		prepare_ErrorFile();
		printer::debug_putstr("In open index.html failed", __FILE__, __FUNCTION__, __LINE__);
		return ;
	}
	else {
		_state = State::READ_FILE;
		_next_state = State::ASSEMBLE;
		printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
		struct pollfd	new_fd;
		new_fd.fd = _fdFile;
		new_fd.events = POLLIN;
		new_fd.revents = 0;
		this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()
		return ;
	}
}

/*
	TODO: take as param error Code
*/
void	Connection::prepare_ErrorFile(void)
{
	_fdFile = open("html/error.html", O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		// TODO: spaeter
		printer::debug_putstr("In open error.html failed", __FILE__, __FUNCTION__, __LINE__);
		generate_internal_server_error_response();
		return ;
	}
	printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
	struct pollfd	new_fd;
	new_fd.fd = _fdFile;
	new_fd.events = POLLIN;
	new_fd.revents = 0;
	this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()

	_current_response.status_code = "404";
	_current_response.headers["Connection"] = "close";
	_state = State::READ_FILE;
	_next_state = State::ASSEMBLE;
	return ;
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

void	Connection::generate_internal_server_error_response(void)
{
	_current_response.headers.clear();
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
		  <h1>Last ALOOOOOOOOO</h1>
		  <p>The server was unable to complete your request. Please try again later.</p>
		</body>
		</html>)";
	_current_response.headers["Content-Length"] = \
		std::to_string(_current_response.file_data.size());
	_state = State::ASSEMBLE;
	return ;
}

void	Connection::set_full_status_code(size_t status)
{
	(void)status;
	// if (_config.error_pages.find(status) == _config.error_pages.end()) {
	// 	status = 500;
	// }
	// _system_path = _config.error_pages[status];
	// prepare_fdFile_param(const std::string status_code);
}

//Utils -- END


