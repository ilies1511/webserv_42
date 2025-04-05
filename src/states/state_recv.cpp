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
	if (bytes <= 0) //TODO: 12.03 check if return of 0 is valid
	{
		printer::debug_putstr("Post recv '<= case'", __FILE__, __FUNCTION__, __LINE__);
		if (bytes == 0) { // Connection closed
			printf("pollserver: socket %d hung up\n", this->_fdConnection);
		}
		else {
			printer::debug_putstr("Pre perror Connection", __FILE__, __FUNCTION__, __LINE__);
			perror("recv");
		}
		std::cout << coloring("Pre ft_closeNcleanRoot()\n", RED);
		ft_closeNcleanRoot(_fdConnection);
		return ;
	}
	size_t old_size = _InputBuffer._buffer.length();
	(void)old_size;
	_InputBuffer._buffer.append(buf, (size_t)bytes);
	#ifdef DEBUG
	std::cout << "Received Data:\n"
				<< std::string(_InputBuffer.data() + old_size, (size_t)bytes) << "\n";
	std::cerr << "cur input:\n" << _InputBuffer._buffer << std::endl;
	#endif
	this->entry_parse();
}
