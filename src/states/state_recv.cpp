#include "Connection.hpp"
#include "Server.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>

bool	Connection::entry_parse(void) {
	if (_request_parser.parse_request_line()) {
		if (_request_parser.parse_headers()) {
			if (_request_parser.parse_body(_server._config.getClientMaxBodySize())) {
				this->request = _request_parser.getRequest();
				//std::cout << this->request << std::endl;
				printer::debug_putstr("Aloo In RECV State - request finished", __FILE__, __FUNCTION__, __LINE__);
				_state = State::PROCESS;
				if (this->request.status_code.has_value()) {
					return (true);
				}
				if (this->_server._config.getServerName().empty()) {
					return (true);
				}
				std::optional<std::string> host = std::nullopt;
				if (this->request.headers.find("host") != this->request.headers.end()) {
					host.emplace(this->request.headers["host"]);
				}
				for (const auto & name : this->_server._config.getServerName()) {
					if (!this->request.status_code.has_value() && host.has_value()) {
						if (name ==  host || name + ":" + std::to_string(this->_server._config.getPort()) == host) {
							return (true);
						}
					}
					if (this->request.uri->host == name || this->request.uri->authority == name) {
						return (true);
					}
				}
				this->request.status_code.emplace(404);
				return (true);
			}
		}
	}
	std::cout << "Request not finished\n";
	return (false);
}

void	Connection::recv_data(void)
{
	if (!check_revent(_fdConnection, POLLIN)) {
		return ;
	}
	_last_activity = std::chrono::steady_clock::now();
	static char buf[BUFFER_SIZE];
	// memset(buf, 0, sizeof(buf));
	ssize_t bytes = recv(_fdConnection, buf, \
						sizeof buf, MSG_DONTWAIT);

	printer::debug_putstr("Post recv", __FILE__, __FUNCTION__, __LINE__);
	if (bytes <= 0)
	{
		printer::debug_putstr("Post recv '<= case'", __FILE__, __FUNCTION__, __LINE__);
		if (bytes == 0) { // Connection closed
			printf("pollserver: socket %d hung up\n", this->_fdConnection);
		}
		else {
			printer::debug_putstr("Pre perror Connection", __FILE__, __FUNCTION__, __LINE__);
			// perror("recv");
		}
		// std::cout << coloring("Pre ft_closeNcleanRoot()\n", RED);
		ft_closeNcleanRoot(_fdConnection);
		return ;
	}
	size_t old_size = _InputBuffer._buffer.length();
	(void)old_size;
	_InputBuffer._buffer.append(buf, (size_t)bytes);
	#ifdef DEBUG
	std::cout << "Received Data:\n"
				<< std::string(_InputBuffer.data() + old_size, (size_t)bytes) << "\n";
	std::cout << "cur input:\n" << _InputBuffer._buffer << std::endl;
	#endif
	this->entry_parse();
}
