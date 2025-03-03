#include "Connection.hpp"

Connection::Connection(int fd, Server& server)
	: _fdConnection(fd), _server(server) , _InputBuffer{}, _OutputBuffer{}
{
	std::cout << "I'm Connection Constructor: cap of buffer: " << _InputBuffer._buffer.capacity() << "Output Buffer: " << _OutputBuffer._buffer.capacity() << "\n";
}

Connection::~Connection(void) {}

int	Connection::getFdConnection(void)
{
	return (_fdConnection);
}


// //Methodes -- BEGIN
// void Connection::handle_input()
// {
// 	switch (_state)
// 	{
// 		case State::READ_HEADER:
// 		{
// 			// Lies Daten in _InputBuffer --> wie Aktuell in Pollserver
// 			ssize_t bytes = recv(_fdConnection, _InputBuffer._buffer.data(), _InputBuffer._buffer.capacity(), 0);
// 			if (bytes <= 0)
// 			{
// 				//TODO: FehlerBehandlung
// 			}
// 			// Versuche, Header zu parsen
// 			if (_parser.parse_header(_InputBuffer, _request))
// 			{
// 				_state = State::READ_BODY;
// 			}
// 			break;
// 		}
// 		case State::READ_BODY:
// 		{
// 			//Read Body woth Content-Length and Chunked-Encoding
// 			if (_parser.parse_body(_InputBuffer, _request))
// 			{
// 				_state = State::PROCESS;
// 			}
// 			break;
// 		}
// 		case State::PROCESS:
// 		{
// 			if (_parser.process())
// 			{
// 				_state = State::WRITE;
// 			}
// 			break;
// 		}
// 		//Unsicher inwiefern man das aufsetzen kann.
// 		case State::CLOSING:
// 		{
// 			_parser.closing();
// 			break;
// 		}
// 	}
// }


//Methodes -- END
