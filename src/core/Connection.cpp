#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>

Connection::Connection(int fd, Server &server)
	:	_state{State::RECV},
		_next_state{State::RECV},
		_server(server),
		request{},
		_current_response{},
		_system_path("html/index.html"),
		_matching_route(std::nullopt),
		_cgi(std::nullopt),
		_fdConnection(fd),
		_fdFile(-1),
		sent_bytes(0),
		finished_sending(false),
		_InputBuffer{},
		_OutputBuffer{},
		_request_parser(_InputBuffer._buffer),
		_autoindex_enabled(false),
		_last_activity(std::chrono::steady_clock::now())
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
			recv_data();
			return ;
		}
		case State::PROCESS:
		{
			// connection_process();
			entry_process();
			return ;
		}
		case State::ASSEMBLE:
		{
			assemble_response();
			return ;
		}
		case State::READ_FILE:
		{
			read_file();
			return ;
		}
		case State::CGI:
		{
			entry_cgi();
			return ;
		}
		case State::SEND:
		{
			send_data();
			return ;
		}
	}
	// assert(0);
	printer::debug_putstr("PRE execute_layer2", __FILE__, __FUNCTION__, __LINE__);
}

// Methodes -- END
