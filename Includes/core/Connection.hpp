#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include "Buffer.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "HTTP_Parser.hpp"
// #include "Server.hpp"
#include <unistd.h>
#include <poll.h>

class Buffer;
class Server;
class Request;
class Response;
// class State;
// class HTTP_Parser;
// class MiddlewareChain;

class Connection
{
	public:
		enum class State { RECV, READ_HEADER, READ_BODY, PROCESS, READ_FILE, WRITE, SEND, CLOSING };
		State				_state;
		Server&				_server;
		Request				request;
		HTTP_Parser			parser;
		Response			_current_response;
	private:
		int					_fdConnection;
		int					_fdFile;
		//TODO:
		// int					_fdWrite;
		// int					_fdRead;
	private:
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
		// State	state() const;
		bool	process_request(const Request &request);
		// std::string	process_request(const Request &request);

		void	handle_input(const int &fd); // --> Koennte auch in den jeweiligen Klassen definiert werden. (Request)
		void	handle_input(void); // --> Koennte auch in den jeweiligen Klassen definiert werden. (Request)
		void	handle_output(void);
		// void	handle_output(int fd);
// --> Koennte auch in den jeweiligen Klassen definiert werden. (Response)
		// void	handle_output(const int &fd); // --> Koennte auch in den jeweiligen Klassen definiert werden. (Response)
		// void	process_request(); // Setzt alles in Gang --> Zentrale Funktion hier
		// void	close();
		int		getFdConnection(void);

		bool	process_request(void);

		void	generate_error_response(Response& response);
		void	generate_response(Response& response);
		void	execute_layer2(void);
		pollfd*	getPollFdElementRoot(int &fd);
		bool	check_revent(int &fd, short rrevent);

	//Methodes -- END
};

#endif
