#include "StaticFileHandler.hpp"

bool StaticFileHandler::serve_file(const std::string &root_dir,
								   const std::string &path,
								   Response &response)
{
	std::string full_path = resolve_path(root_dir, path);

	if (!is_valid_path(root_dir, full_path))
	{
		response.status_code = "403";
		return false;
	}
	struct stat st;
	if (stat(full_path.c_str(), &st) != 0)
	{
		response.status_code = "404";
		return false;
	}
	if (S_ISDIR(st.st_mode))
	{
		if (!handle_directory(full_path, response))
		{
			response.status_code = "403";
			return false;
		}
	}
	return read_file_content(full_path, static_cast<size_t>(st.st_size), response);
}

std::string StaticFileHandler::resolve_path(const std::string &root, const std::string &path)
{
	if (path.find("..") != std::string::npos)
		return "";
	return root + path;
}

bool StaticFileHandler::is_valid_path(const std::string &root, const std::string &path)
{
	return path.find(root) == 0;
}

bool StaticFileHandler::handle_directory(const std::string &path, Response &response)
{
	// TODO: Hier könnte man directory listing implementieren
	(void)path;
	(void)response;
	return (false);
}

bool StaticFileHandler::read_file_content(const std::string &path, size_t file_size, Response &response)
{
	//TODO: check if blocking
	std::ifstream file(path, std::ios::binary);
	if (!file)
		return false;

	response.headers["Content-Type"] = get_mime_type(path);
	response.headers["Content-Length"] = std::to_string(file_size);

	response.body.resize(file_size);
	file.read(&response.body[0], static_cast<long>(file_size));
	return true;
}

std::string StaticFileHandler::get_mime_type(const std::string &path)
{
	size_t dot = path.find_last_of('.');
	if (dot == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dot);
	if (ext == ".html")
		return "text/html";
	if (ext == ".css")
		return "text/css";
	if (ext == ".js")
		return "application/javascript";
	if (ext == ".png")
		return "image/png";
	if (ext == ".jpg")
		return "image/jpeg";
	return "text/plain";
}

// bool StaticFileHandler::send_file_directly(int out_fd, const std::string& path,
// 	Response& response)
// {
// 	int in_fd = open(path.c_str(), O_RDONLY);
// 	if (in_fd == -1) return false;

// 	struct stat st;
// 	fstat(in_fd, &st);

// 	response.headers["Content-Type"] = get_mime_type(path);
// 	response.headers["Content-Length"] = std::to_string(st.st_size);

// 	// Header senden
// 	std::string headers = build_headers(response);
// 	send(out_fd, headers.c_str(), headers.size(), 0);

// 	// Datei direkt senden
// 	off_t offset = 0;
// 	sendfile(out_fd, in_fd, &offset, st.st_size);
// 	close(in_fd);
// 	return (true);
// }
