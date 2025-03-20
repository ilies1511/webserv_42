#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

void	Connection::read_file(void)
{
	/*
		1. PollValue des File_Fd checken, in diesem Fall auf POLLIN
		2. wenn nicht ready return
		3. Wenn ready: In permanent Buffer reinlesen => der Body der Response
			(separat vom recv und vom Response_string) um damit die
			Response zu generieren, weil man nicht in die Zukfunft schauen kann.
				--> Ziel: content-lenght zu ermittlen und dann Body ran-appenden (aus permanten Body Buffer)
			Alternativ:
			auch mit stat() moelgich
	*/
	/*
		//TODO: Imrove Handling --> File should be opened somewhere else already
		For example in Process, where first it's checked whether File should
		be opened
	*/
	if (!check_revent(_fdFile, POLLIN)) {
		printer::debug_putstr("Event failed", __FILE__, __FUNCTION__, __LINE__);
		return ;
	}
	// response->file_data = read(file_fd);
	/*
		TODO: gelesenen bytes sollen irgendwie in das response.file_data rein
	*/
	// char read_file_buffer[4090];
	printer::Header("In READ_FILE + next_state" + Connection::to_string(_next_state));

	Buffer	read_file_buffer;
	ssize_t bytes_read = read(_fdFile, read_file_buffer._buffer.data(), 4090);

	if (bytes_read <= 0)
	{
		printer::debug_putstr("bytes_read <= 0 Case", __FILE__, __FUNCTION__, __LINE__);
		if (bytes_read == 0) {
			_server.ft_closeNclean(_fdFile);
			std::cout << "Set _fdFile: " << _fdFile << " to -1\n";
			_fdFile = -1;
			// close (_fdFile);
			_state = State::PROCESS;
			_current_response.FileData = true;
			// _state = State::SEND;
			_current_response.file_data.assign(read_file_buffer._buffer.begin(), read_file_buffer._buffer.end());
			_state = _next_state;
			return ;
			//File ist fertig gelesen
		}
		else if (bytes_read < 0) {
			assert(0);
			_server.ft_closeNclean(_fdFile);
			return ;
			//TODO: Improve & understand propor ERROR behavior when occures --> to fix
		}
	}
	else
	{
		//TODO: file not fully read
		printer::debug_putstr("bytes_read > 0 Case PRE SEND", __FILE__, __FUNCTION__, __LINE__);
		_state = _next_state;
		_current_response.file_data.assign(read_file_buffer._buffer.begin(), read_file_buffer._buffer.end());
		//TODO: improve Case is not fully read --> go Back to process and reread again until end of file
		_current_response.FileData = true;
		_server.ft_closeNclean(_fdFile);
		std::cout << "Set _fdFile: " << _fdFile << " to -1\n";
		_fdFile = -1;
		return ;
	}

	/* TODO: 17.03.25 - Improve Handling, sieh structur file
		// if (finished)
		// {
		// 	close(file_fd);
		// 	// _server.ft_closeNclean(_fdFile); // --> ist das gut, hier den Clean-Up zu machen
		// 	_state = State::PROCESS
		// 	return ;
		// }
	*/
}
