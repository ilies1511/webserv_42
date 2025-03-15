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

//Utils -- END
