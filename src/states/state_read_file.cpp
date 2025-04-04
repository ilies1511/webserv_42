#include "Connection.hpp"
#include "Server.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>

/*
	dd if=/dev/zero of=1GB_file bs=1M count=1024 --> creates 1GB file in pwd
*/

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
		std::cout << coloring("File FD: " + std::to_string(_fdFile) + "\n", LIGHT_BLUE);
		_server.ft_closeNclean(_fdFile);
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
		// _fdFile = -1;
		set_full_status_code(500);
		return;
	}
	else if (bytes_read == 0) {
		// EOF - Datei vollständig gelesen --> nur hier gehts 'weiter'
		_server.ft_closeNclean(_fdFile);
		// _fdFile = -1;
		_current_response.FileData = true;
		_state = _next_state;
		return;
	}
	else {
		if (_current_response.file_data.size() + (size_t)bytes_read > MAX_FILE_SIZE) {
			P_DEBUG("read_file Case MAX_FILE_SIZE Case\n");
			_server.ft_closeNclean(_fdFile);
			// _fdFile = -1;
			set_full_status_code(413); // 413 Payload Too Large
			return;
		}
		_current_response.file_data.append(buf, (size_t)bytes_read);
	}
}
