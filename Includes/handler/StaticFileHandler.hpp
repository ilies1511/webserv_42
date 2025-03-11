#ifndef StaticFileHandler_HPP
#define StaticFileHandler_HPP

#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>
#include "Response.hpp"
//sendfile()
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

class StaticFileHandler
{
	public:
		static bool serve_file(const std::string& root_dir,
							   const std::string& path, Response& response);
		// static bool send_file_directly(int out_fd, const std::string& path,
		// 							   Response& response);
	private:
		static std::string resolve_path(const std::string& root, const std::string& path);
		static bool is_valid_path(const std::string& root, const std::string& path);
		static bool handle_directory(const std::string& path, Response& response);
		static bool read_file_content(const std::string& path, size_t file_size, Response& response);
		static std::string get_mime_type(const std::string& path);
	};

#endif
