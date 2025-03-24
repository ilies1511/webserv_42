#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include "Buffer.hpp"
// #include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "HTTP_Parser.hpp"
// #include "Server.hpp"
#include <unistd.h>
#include <poll.h>
#include <filesystem>

class Buffer;
class Server;
// class Request;
class Response;
// class State;
// class HTTP_Parser;
// class MiddlewareChain;

class Connection
{
	public:
		// enum class State { RECV, READ_HEADER, READ_BODY, PROCESS, READ_FILE, WRITE, SEND, CLOSING };
		enum class State { RECV, PROCESS, READ_FILE, WRITE, SEND, ASSEMBLE};
		static std::string to_string(State state) {
			switch (state) {
				case State::RECV: return "RECV";
				case State::PROCESS: return "PROCESS";
				case State::READ_FILE: return "READ_FILE";
				case State::WRITE: return "WRITE";
				case State::SEND: return "SEND";
				case State::ASSEMBLE: return "ASSEMBLE";
				default: return "UNKNOWN";
			}
		}
		State					_state;
		State					_next_state;
		Server&					_server;
		Request					request;
		// Parser				parser;
		Response				_current_response;
		std::string				_system_path;
	private:
		int						_fdConnection;
		int						_fdFile; //TODO: add error_fd
		ssize_t					sent_bytes;
		bool					finished_sending;
	public:
		Buffer					_InputBuffer;//TODO: 19.03.25 change to lowercase // Dieser Buffer wird fuer read() bzw. recv() verwendet
		Buffer					_OutputBuffer; // Dieser Buffer wird fuer write bzw. send() verwendet
		RequestParser			_request_parser;
		bool					_autoindex_enabled = false;
		std::filesystem::path	_full_path = {};
		//TODO:
		// int					_fdWrite;
		// int					_fdRead;

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
		void	ft_closeNcleanRoot(int &fd);
		void	print_request_data(Request &request);
		void	prepare_fdFile(void);
		void	prepare_ErrorFile(void);
		// void	assemble_response(Response &response);
		void	assemble_response(void);
		void	assemble_response2(void);
		void	generate_internal_server_error_response(void);
		void	entry_process(void);
		void	methode_handler(void);
		void	handle_get(void);
		void	handle_post(void);
		void	handle_delete(void);
		void	read_file(void);
		void	write_file(void);
		void	send_data(void);
		void	connection_process(void);
		void	recv_data(void);
		bool	file_exists_and_readable(const std::filesystem::path& p);
		void	generate_autoindex(const std::filesystem::path& dir);
		void	prepare_fdFile_param(const std::string status_code);
		bool	entry_parse(void);
		void	trailing_slash_case(void);
		void	no_trailing_slash_case(void);
	//Methodes -- END
};

#endif
