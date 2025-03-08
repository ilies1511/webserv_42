#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"

Connection::Connection(int fd, Server& server)
	: _state{State::READ_HEADER}, _server(server),  _fdConnection(fd) , _InputBuffer{}, _OutputBuffer{}
{
	std::cout << "I'm Connection Constructor: cap of buffer: " << _InputBuffer._buffer.capacity() << "Output Buffer: " << _OutputBuffer._buffer.capacity() << "\n";
}

Connection::~Connection(void) {}

int	Connection::getFdConnection(void)
{
	return (_fdConnection);
}

// //Methodes -- BEGIN

// /*
// 	TODO: Finish-Up Parser - Request - Response Handling unsing Middleware Chain
// 	Zunaechst aber ohne Middleware sondern so einfach wie moeglich, mit dem Ziel
// 	etwas zum laufen zu bringen
// */
// bool	Connection::process_request(void)
// {
// 	MiddlewareChain	chain;

// 	chain.add(std::make_unique<LoggingMiddleware>())
// 	if (!chain.execute(request_, response_))
// 		return
// 	reactor_.router().route(request_, response_);
// }


bool	Connection::process_request(const Request &request)
{
	Response	response(request);

	response.status_code = "200";
	response.body =	"HTTP/1.1 200 OK\r\n"
					"Content-Type: text/plain\r\n"
					"Content-Length: 13\r\n\r\n"
					"Hello World!\n";

	_OutputBuffer._buffer.assign(response.body.begin(), response.body.end());
	return (true);

	// if (request._method == "GET" && request._uri == "/")
	// {
	// 	return ("HTTP/1.1 200 OK\r\n"
	// 		"Content-Type: text/html\r\n"
	// 		"Content-Length: 48\r\n"
	// 		"\r\n"
	// 		"<html><body><h1>Hallo Welt!</h1></body></html>");
	// }
	// //TODO: Handle more Cases
	// return ("HTTP/1.1 404 Not Found\r\n"
	// 	"Content-Type: text/html\r\n"
	// 	"Content-Length: 45\r\n"
	// 	"\r\n"
	// 	"<html><body><h1>Seite nicht gefunden</h1></body></html>");
}


// send("GET /text.txt ")
// sleep(1)
// send("HTTP:1.1\r\n\r\n")



//Zentrale - EntryPoint fuer Input
void Connection::handle_input(const int &fd)
{
	(void)fd;
	if (_server._pollfds[_server._i].revents & (POLLIN | POLLHUP))
	{
	switch (_state)
	{
		case State::READ_HEADER:
		{
			// Lies Daten in _InputBuffer --> wie Aktuell in Pollserver
			ssize_t bytes = \
				recv(_fdConnection, _InputBuffer._buffer.data(), _InputBuffer._buffer.capacity() - 1, 0);
			_InputBuffer._buffer[(size_t)bytes] = 0;
			printer::debug_putstr("Post recv", __FILE__, __FUNCTION__, __LINE__);
			if (bytes <= 0)
			{
				printer::debug_putstr("Post recv '<= case'", __FILE__, __FUNCTION__, __LINE__);
				if (bytes == 0)
				{
					// Connection closed
					printf("pollserver: socket %d hung up\n", this->_fdConnection);
				}
				else
				{
					printer::debug_putstr("Pre perror Connection", __FILE__, __FUNCTION__, __LINE__);
					perror("recv");
				}
				_server.ft_closeNclean(_fdConnection);
				// _server.ft_closeNclean(_server._i);

				// _server._i--;
			}
			else
			{
				std::cout << "Received Data:\n" << std::string(_InputBuffer.data(), (size_t)bytes) << "\n";
				// Versuche, Header zu parsen
				if (parser.parse_header(_InputBuffer, request))
				{
					printer::debug_putstr("Pre recv", __FILE__, __FUNCTION__, __LINE__);
					_state = State::READ_BODY;
				}
				if (strlen(_InputBuffer._buffer.data()) > 10)
				{
					/*TODO:
					// open and read text.txt with polling the read calls on the text.txt fd
					// set content as data to send to connection fd
					*/
					;
				}
			}
			break;
		}
		case State::READ_BODY:
		{
			printer::debug_putstr("Pre Parse Body", __FILE__, __FUNCTION__, __LINE__);
			//Read Body woth Content-Length and Chunked-Encoding
			if (parser.parse_body(_InputBuffer, request))
			{
				_state = State::PROCESS;
				printer::debug_putstr("POST Parse Body", __FILE__, __FUNCTION__, __LINE__);
			}
			break;
		}
		case State::PROCESS:
		{
			/*
				//TODO: States, nicht das einzige, noch Abhanenig von
				z.B PollValue, aber auch Files/Pipes/Lesen/Schreiben
				if (_pollfds[_i].revents & (POLLIN | POLLHUP)
					DANN ERRST PROCESSING
			*/
			// if (parser.process_request())

			/*
				TODO: check here
				// if (_pollfds[_i].revents & POLL_OUT)
			*/

			if (this->process_request(request))
			{
				printer::debug_putstr("In entry Point Handle Input PROCESS", __FILE__, __FUNCTION__, __LINE__);
				_state = State::WRITE;
				std::cout << "Pre enable Output \n";
				// _server.enable_output(this->_fdConnection);
				std::cout << "POST enable Output \n";
			}
			_state = State::WRITE;
			printer::debug_putstr("Post PROCESS", __FILE__, __FUNCTION__, __LINE__);
			break;
		}
		case State::WRITE:
		{
			_state = State::CLOSING;
			printer::debug_putstr("PRE WRITE", __FILE__, __FUNCTION__, __LINE__);
			//Muss ich hier explizit handle_output callen ?
			this->handle_output();
			// this->handle_output();
			printer::debug_putstr("POST WRITE handle_output", __FILE__, __FUNCTION__, __LINE__);
			break;
		}
		case State::CLOSING:
		{
			_state = State::READ_HEADER; // Keep Alive Loop
			// parser.closing();

			// _server.ft_closeNclean(_fdConnection);

			// _server.ft_closeNclean(_server._i);
			// _server._i--;
			break;
		}
	}
	return ;
	}
}

// // Backup Stand 05.03.2025 11:30
// void	Connection::handle_input(void)
// {
// 	if (parser.parse(_InputBuffer, request))
// 	{
// 		/*
// 			TODO:(todel HardCode):
// 		*/
// 		std::cout << __FUNCTION__ << " " << __LINE__ << " " \
// 			<< "request method: " << request._method << "request uri: " \
// 			<< request._uri << "\n";
// 		std::string response_str = process_request(request);

// 		// _OutputBuffer._buffer.insert(_OutputBuffer._buffer.end(), response_str.begin(), response_str.end());

// 		std::cout << "buf len: " << this->_OutputBuffer.size() << "\n";
// 		std::cout << "buf content: " << this->_OutputBuffer.data() << "\n";
// 		if (send(this->_fdConnection, _OutputBuffer.data(), _OutputBuffer.size(), 0) == -1)
// 		{
// 			perror("send");
// 		}
// 		//TODO: Finish up Cycle of Request & Response
// 		// response.process_request();
// 	}
// }

void	Connection::handle_output(void)
{
	printer::debug_putstr("PRE in handle_output", __FILE__, __FUNCTION__, __LINE__);
	ssize_t sent = send(this->_fdConnection , this->_OutputBuffer.data(), _OutputBuffer._buffer.size(), 0);
	if(sent > 0)
	{
		std::cout << "Sent Response:\n" << std::string(_OutputBuffer.data(), (size_t)sent) << "\n";
		this->_OutputBuffer._buffer.clear();
		// _state = State::CLOSING; // Oder READ_HEADER für Keep-Alive damit circular ist
	}
	printer::debug_putstr("POST in handle_output", __FILE__, __FUNCTION__, __LINE__);
}

// Methodes -- END
