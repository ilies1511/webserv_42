#include "Connection.hpp"
#include "Server.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>

/*
	dd if=/dev/zero of=1GB_file bs=1M count=1024 --> creates 1GB file in pwd
*/

// void	Connection::read_file(void)
// {
// 	/*
// 		1. PollValue des File_Fd checken, in diesem Fall auf POLLIN
// 		2. wenn nicht ready return
// 		3. Wenn ready: In permanent Buffer reinlesen => der Body der Response
// 			(separat vom recv und vom Response_string) um damit die
// 			Response zu generieren, weil man nicht in die Zukfunft schauen kann.
// 				--> Ziel: content-lenght zu ermittlen und dann Body ran-appenden (aus permanten Body Buffer)
// 			Alternativ:
// 			auch mit stat() moelgich
// 	*/

// 	if (!check_revent(_fdFile, POLLIN)) {
// 		printer::debug_putstr("Event failed", __FILE__, __FUNCTION__, __LINE__);
// 		return ;
// 	}

// 	printer::Header("In READ_FILE + next_state" + Connection::to_string(_next_state));

// 	//TODO: 23.03 Improve hotfix
// 	Buffer	read_file_buffer;
// 	static char buf[BUFFER_SIZE];
// 	// memset(buf, 0, sizeof(buf));
// 	ssize_t bytes_read = read(_fdFile, buf, sizeof buf);
// 	if (bytes_read <= 0)
// 	{
// 		printer::debug_putstr("bytes_read <= 0 Case", __FILE__, __FUNCTION__, __LINE__);
// 		if (bytes_read == 0) {
// 			_server.ft_closeNclean(_fdFile);
// 			std::cout << "Set _fdFile: " << _fdFile << " to -1\n";
// 			_fdFile = -1;
// 			_state = State::PROCESS;
// 			_current_response.FileData = true;
// 			_current_response.file_data.assign(read_file_buffer._buffer.begin(), read_file_buffer._buffer.end());
// 			_state = _next_state;
// 			return ;
// 			//File ist fertig gelesen
// 		}
// 		else if (bytes_read < 0) {
// 			assert(0);
// 			_server.ft_closeNclean(_fdFile);
// 			return ;
// 			//TODO: Improve & understand propor ERROR behavior when occures --> to fix
// 		}
// 	}
// 	else
// 	{
// 		read_file_buffer._buffer.append(buf, (size_t)bytes_read);
// 		//TODO: file not fully read
// 		printer::debug_putstr("bytes_read > 0 Case PRE SEND", __FILE__, __FUNCTION__, __LINE__);
// 		_state = _next_state;
// 		_current_response.file_data.assign(read_file_buffer._buffer.begin(), read_file_buffer._buffer.end());
// 		//TODO: improve Case is not fully read --> go Back to process and reread again until end of file
// 		_current_response.FileData = true;
// 		_server.ft_closeNclean(_fdFile);
// 		std::cout << "Set _fdFile: " << _fdFile << " to -1\n";
// 		_fdFile = -1;
// 		return ;
// 	}
// }


// void Connection::read_file(void)
// {
// 	if (!check_revent(_fdFile, POLLIN)) {
// 		printer::debug_putstr("Event not ready", __FILE__, __FUNCTION__, __LINE__);
// 		return;
// 	}

// 	static char buf[BUFFER_SIZE];
// 	ssize_t bytes_read = read(_fdFile, buf, sizeof(buf));

// 	if (bytes_read < 0) {
// 		perror("read");
// 		_server.ft_closeNclean(_fdFile);
// 		_fdFile = -1;
// 		set_full_status_code(500);
// 		return ;
// 	}
// 	else if (bytes_read == 0) {
// 		// EOF - Datei vollständig gelesen
// 		_server.ft_closeNclean(_fdFile);
// 		_fdFile = -1;
// 		_current_response.FileData = true;
// 		_state = _next_state;
// 		return ;
// 	}
// 	else {
// 		_current_response.file_data.insert(
// 			_current_response.file_data.end(), buf, buf + bytes_read);
// 	}
// }


static uintmax_t get_file_size(const std::string& path)
{
	try {
		return std::filesystem::file_size(path);
	} catch (std::filesystem::filesystem_error& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return (0);
	}
}

void Connection::read_file(void)
{
	//Early Exit -- BEGIN
	if (!check_revent(_fdFile, POLLIN)) {
		P_DEBUG("Event not ready\n");
		return;
	}
	const size_t MAX_FILE_SIZE = _server._config.getClientMaxBodySize();
	const size_t FILE_SIZE_CHECK_THRESHOLD = 100 * 1024 * 1024; // 100MB
	if (MAX_FILE_SIZE == 0 || (MAX_FILE_SIZE > 0 && \
			MAX_FILE_SIZE < FILE_SIZE_CHECK_THRESHOLD
			&& get_file_size(this->_system_path) > MAX_FILE_SIZE)) {
		P_DEBUG("Aloooo In read_file EARLY-Exit Case\n");
		set_full_status_code(413);
		return ;
	}
	//Early Exit -- END

	static char buf[BUFFER_SIZE];
	ssize_t bytes_read;

	do {
		bytes_read = read(_fdFile, buf, sizeof(buf));
	} while (bytes_read < 0 && errno == EINTR);

	if (bytes_read < 0) {
		perror("read");
		_server.ft_closeNclean(_fdFile);
		_fdFile = -1;
		set_full_status_code(500);
		return;
	}
	else if (bytes_read == 0) {
		// EOF - Datei vollständig gelesen --> nur hier gehts 'weiter'
		_server.ft_closeNclean(_fdFile);
		_fdFile = -1;
		_current_response.FileData = true;
		_state = _next_state;
		return;
	}
	else {
		if (_current_response.file_data.size() + (size_t)bytes_read > MAX_FILE_SIZE) {
			P_DEBUG("read_file Case MAX_FILE_SIZE Case\n");
			_server.ft_closeNclean(_fdFile);
			_fdFile = -1;
			set_full_status_code(413); // 413 Payload Too Large
			return;
		}
		_current_response.file_data.append(buf, (size_t)bytes_read);
	}
}
