#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
// #include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

bool	Connection::entry_parse(void) {
	if (_request_parser.parse_request_line()) {
		if (_request_parser.parse_headers()) {
			// in real webserv would select config here
			// 100 is a placeholder for value from the config
			if (_request_parser.parse_body(100)) {
				// save to get request here
				this->request = _request_parser.getRequest();
				std::cout << this->request << std::endl;
				printer::debug_putstr("Aloo In RECV State - request finished", __FILE__, __FUNCTION__, __LINE__);
				_state = State::PROCESS;
				return (true);
			}
		}
	}
	std::cout << "Enough, Request finished\n";
	return (false);
}

void	Connection::recv_data(void)
{
	if (!check_revent(_fdConnection, POLLIN)) {
		return ;
	}
	//Empfehlung: von dem Buffer in einen permanten reinkopieren --> simpler, Problem: _InputBuffer._buffer.capacity
	static char buf[10000000];
	// ssize_t bytes = recv(_fdConnection, _InputBuffer._buffer.data(), \
	// 					_InputBuffer._buffer.capacity() - 1, 0);
	//_InputBuffer._buffer[(size_t)bytes] = 0;
	memset(buf, 0, sizeof(buf));
	ssize_t bytes = recv(_fdConnection, buf, \
						sizeof buf - 1, 0);
	buf[(size_t)bytes] = 0;
	std::string str(buf);
	_InputBuffer._buffer.assign(str.begin(), str.end());
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
		std::cout << coloring("Pre ft_closeNcleanRoot()\n", RED);//TODO: statt clean hier, erst nach der current loop interation z.B mit clean_after
		ft_closeNcleanRoot(_fdConnection);
		return ;
	}
	std::cout << "Received Data:\n"
				<< std::string(_InputBuffer.data(), (size_t)bytes) << "\n";
	this->entry_parse();
	// //ENTRY-Point Parser --> fills Request Instance (Instance belongs to Connection)
	// parser.entry_parse(this->_InputBuffer, request);
	// printer::debug_putstr("In RECV State - Parser finished", __FILE__, __FUNCTION__, __LINE__);
	// std::cout << " Method: " \
	// 			<< request._method << " URI:" \
	// 			<< request._uri \
	// 			<< " Version: " << request._version \
	// 			<< " Finished: " << request.is_finished \
	// 			<< "\n";
	// if (this->request.is_finished)
	// {
	// 	printer::debug_putstr("Aloo In RECV State - request finished", __FILE__, __FUNCTION__, __LINE__);
	// 	_state = State::PROCESS;
	// 	return ;
	// }
	// else {
	// 	std::cout << "Enough, Request finished\n";
	// 	return ;
	// }
}
