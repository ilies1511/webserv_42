#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include "Buffer.hpp"
#include <unistd.h>

class Buffer;
class Server;
// class Request;
// class Response;
// class State;
// class HTTP_Parser;
// class MiddlewareChain;

class Connection
{
	public:
	enum class State { READ_HEADER, READ_BODY, PROCESS, WRITE, CLOSING };
	private:
	int					_fdConnection;
	Server&				_server;
	State				_state;
	// Request				_request;
	// Response			_response;
	// HTTP_Parser			_parser; //vielleicht schlauer, wenn in request gecallt wird.
	// MiddlewareChain*	_middlewares;
	// Router*				_router;

	//OCF -- BEGIN
	public:
		Connection(int fd, Server& server);
		~Connection(void);
	private:
		Connection(void) = delete;
		Connection(const Connection& other) = delete;
		Connection& operator=(const Connection& other) = delete;
	//OCF -- END

	public:
		Buffer				_InputBuffer; // Dieser Buffer wird fuer read() bzw. recv() verwendet
		Buffer				_OutputBuffer; // Dieser Buffer wird fuer write bzw. send() verwendet

		//Utils -- BEGIN
		//Utils -- END
	//Methodes -- BEGIN
	private:
		//Wird von process_request gecallt
		// void	handle_input(); // --> Koennte auch in den jeweiligen Klassen definiert werden. (Request)
		// void	handle_output(); // --> Koennte auch in den jeweiligen Klassen definiert werden. (Response)
	public:
		State	state() const;
		void	handle_input(void); // --> Koennte auch in den jeweiligen Klassen definiert werden. (Request)
		void	handle_output(void); // --> Koennte auch in den jeweiligen Klassen definiert werden. (Response)
		// void	process_request(); // Setzt alles in Gang --> Zentrale Funktion hier
		// void	close();
		int		getFdConnection(void);
	//Methodes -- END
};

#endif
