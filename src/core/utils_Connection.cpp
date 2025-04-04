#include "Connection.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <optional>

//Utils -- BEGIN
pollfd*	Connection::getPollFdElementRoot(int &fd)
{
	for (auto& p : _server._core._pollfds) {
		if (p.fd == fd) {
			return &p;
		}
	}
	return (nullptr);
}

bool	Connection::check_revent(int &fd, short rrevent)
{
	pollfd	*pfd = getPollFdElementRoot(fd);

	return (pfd && pfd->revents & rrevent);
}

void	Connection::ft_closeNcleanRoot(int &fd)
{
	_server.ft_closeNclean(fd);
}

void Connection::generate_error_response(Response &response)
{
	const std::map<std::string, std::string> status_texts =
		{
			{"400", "Bad Request"},
			{"403", "Forbidden"},
			{"404", "Not Found"},
			{"500", "Internal Server Error"}};
	response.body = "HTTP/1.1 " + response.status_code + " " +
					status_texts.at(response.status_code) + "\r\n"
															"Content-Type: text/html\r\n"
															"Content-Length: " +
					std::to_string(response.body.size()) + "\r\n"
														   "\r\n"
														   "<html><body><h1>" +
					response.status_code + " - " +
					status_texts.at(response.status_code) + "</h1></body></html>";
}

void	Connection::prepare_fdFile_param(const std::string status_code)
{
	_current_response.status_code = status_code;

	std::cout << coloring("\n\nin prepare_fdFile - _system_path: " + _system_path + "\n", TURQUOISE);
	_fdFile = open(_system_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		prepare_ErrorFile();
		printer::debug_putstr("In open index.html failed", __FILE__, __FUNCTION__, __LINE__);
		return ;
	}
	else {
		_state = State::READ_FILE;
		_next_state = State::ASSEMBLE;
		printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
		struct pollfd	new_fd;
		new_fd.fd = _fdFile;
		new_fd.events = POLLIN;
		new_fd.revents = 0;
		this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()
		return ;
	}
}

void	Connection::prepare_fdFile(void)
{
	std::cout << coloring("\n\nin prepare_fdFile - _system_path: " + _system_path + "\n", TURQUOISE);
	_fdFile = open(_system_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		prepare_ErrorFile();
		printer::debug_putstr("In open index.html failed", __FILE__, __FUNCTION__, __LINE__);
		return ;
	}
	else {
		_state = State::READ_FILE;
		_next_state = State::ASSEMBLE;
		printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
		struct pollfd	new_fd;
		new_fd.fd = _fdFile;
		new_fd.events = POLLIN;
		new_fd.revents = 0;
		this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()
		return ;
	}
}

/*
	TODO: take as param error Code
*/
void	Connection::prepare_ErrorFile(void)
{
	_fdFile = open("html/error.html", O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		// TODO: spaeter
		printer::debug_putstr("In open error.html failed", __FILE__, __FUNCTION__, __LINE__);
		generate_internal_server_error_response();
		return ;
	}
	printer::debug_putstr("In open index.html success", __FILE__, __FUNCTION__, __LINE__);
	struct pollfd	new_fd;
	new_fd.fd = _fdFile;
	new_fd.events = POLLIN;
	new_fd.revents = 0;
	this->_server.add_to_pollfds_prefilled(new_fd); // TODO: add_to_pollfds_prefilledRoot()

	_current_response.status_code = "404";
	_current_response.headers["Connection"] = "close";
	_state = State::READ_FILE;
	_next_state = State::ASSEMBLE;
	return ;
}

void	Connection::generate_internal_server_error_response(void)
{
	_current_response.headers.clear();
	_current_response.http_version = "HTTP/1.1";
	_current_response.status_code = "500";
	_current_response.headers["Content-Type"] = "text/html";
	_current_response.headers["Connection"] = "close";
	_current_response.file_data = \
		R"(<!doctype html>
		<html lang="en">
		<head>
		  <title>500 Internal Server Error</title>
		</head>
		<body>
		  <h1>Last ALOOOOOOOOO</h1>
		  <p>The server was unable to complete your request. Please try again later.</p>
		</body>
		</html>)";
	_current_response.headers["Content-Length"] = \
		std::to_string(_current_response.file_data.size());
	_state = State::ASSEMBLE;
	return ;
}

// map_statuscode_to_file
void	Connection::set_full_status_code(size_t status, \
		std::optional<std::string> custom_path)
{
	if (_server._config.getErrorPages().find(status) == _server._config.getErrorPages().end()) {
		status = 500;
		std::cout << "Alo 1\n";
	}
	_current_response.status_code = std::to_string(status);
	// Wenn custom_path vorhanden ist, benutze es; andernfalls nutze den Standardpfad
	if (custom_path.has_value()) {
		std::cout << "Alo 2\n";
		_system_path = custom_path.value();
	}
	else {
		std::cout << "Alo 3\n";
		std::cout << "Status: " << status << "PRE System Path: " << _system_path << "\n";
		_system_path = _server._config.getErrorPages()[status]; //TODO: use getErrorPages().at(status) --> but throws exception
		std::cout << "System Path: " << _system_path << "\n";
	}
	prepare_fdFile();
}

bool Connection::is_timed_out() const
{
	auto now = std::chrono::steady_clock::now();
	return (now - _last_activity) > TIMEOUT_DURATION;
}

std::string Connection::get_mime_type(const std::string &path)
{
	size_t dot = path.find_last_of('.');
	if (dot == std::string::npos)
		return "application/octet-stream";

	std::string ext = path.substr(dot);
	std::transform(ext.begin(), ext.end(), ext.begin(),
					[](unsigned char c) { return std::tolower(c); });

	static const std::unordered_map<std::string, std::string> mimeTypes = {
		{".jpg",  "image/jpeg"},
		{".webm", "video/webm"},
		{".ogg",  "video/ogg"},
		{".mov",  "video/quicktime"},
		{".aac", "audio/aac"},
		{".abw", "application/x-abiword"},
		{".apng", "image/apng"},
		{".arc", "application/x-freearc"},
		{".avif", "image/avif"},
		{".avi", "video/x-msvideo"},
		{".azw", "application/vnd.amazon.ebook"},
		{".bin", "application/octet-stream"},
		{".bmp", "image/bmp"},
		{".bz", "application/x-bzip"},
		{".bz2", "application/x-bzip2"},
		{".cda", "application/x-cdf"},
		{".csh", "application/x-csh"},
		{".css", "text/css"},
		{".csv", "text/csv"},
		{".doc", "application/msword"},
		{".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
		{".eot", "application/vnd.ms-fontobject"},
		{".epub", "application/epub+zip"},
		{".gz", "application/gzip"},
		{".gif", "image/gif"},
		{".htm", "text/html"},
		{".html", "text/html"},
		{".ico", "image/vnd.microsoft.icon"},
		{".ics", "text/calendar"},
		{".jar", "application/java-archive"},
		{".jpeg", "image/jpeg"},
		{".js", "text/javascript"},
		{".json", "application/json"},
		{".jsonld", "application/ld+json"},
		{".mid", "audio/midi, audio/x-midi"},
		{".mjs", "text/javascript"},
		{".mp3", "audio/mpeg"},
		{".mp4", "video/mp4"},
		{".mpeg", "video/mpeg"},
		{".mpkg", "application/vnd.apple.installer+xml"},
		{".odp", "application/vnd.oasis.opendocument.presentation"},
		{".ods", "application/vnd.oasis.opendocument.spreadsheet"},
		{".odt", "application/vnd.oasis.opendocument.text"},
		{".oga", "audio/ogg"},
		{".ogv", "video/ogg"},
		{".ogx", "application/ogg"},
		{".opus", "audio/ogg"},
		{".otf", "font/otf"},
		{".png", "image/png"},
		{".pdf", "application/pdf"},
		{".php", "application/x-httpd-php"},
		{".ppt", "application/vnd.ms-powerpoint"},
		{".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
		{".rar", "application/vnd.rar"},
		{".rtf", "application/rtf"},
		{".sh", "application/x-sh"},
		{".svg", "image/svg+xml"},
		{".tar", "application/x-tar"},
		{".tif", "image/tiff"},
		{".ts", "video/mp2t"},
		{".ttf", "font/ttf"},
		{".txt", "text/plain"},
		{".vsd", "application/vnd.visio"},
		{".wav", "audio/wav"},
		{".weba", "audio/webm"},
		{".webm", "video/webm"},
		{".webp", "image/webp"},
		{".woff", "font/woff"},
		{".woff2", "font/woff2"},
		{".xhtml", "application/xhtml+xml"},
		{".xls", "application/vnd.ms-excel"},
		{".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
		{".xml", "application/xml"},
		{".xul", "application/vnd.mozilla.xul+xml"},
		{".zip", "application/zip"},
		{".3gp", "video/3gpp"},
		{".3g2", "video/3gpp2"},
		{".7z", "application/x-7z-compressed"},
		{".flac", "audio/flac"},
		{".m4a", "audio/mp4"},
		{".mkv", "video/x-matroska"}
	};

	auto it = mimeTypes.find(ext);
	if (it != mimeTypes.end()) {
		return it->second;
	}
	// Default: entweder text/plain oder application/octet-stream
	// return "application/octet-stream";
	return "application/octet-stream";
}

//Cookie-Specific HelperFncs --BEGIN
void	Connection::check_cgi_response_for_cookies(void)
{
	for (;;)
	{
		break ;
	}
}

static std::string trim(const std::string& s) {
	auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) {
		return (std::isspace(ch));
	});
	auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) {
		return (std::isspace(ch));
	}).base();
	return ((start < end) ? std::string(start, end) : std::string());
}

std::vector<std::string> Connection::cookie_split(const std::string& cookie_header) {
	std::vector<std::string> cookies;
	std::istringstream tokenStream(cookie_header);
	std::string token;

	while (std::getline(tokenStream, token, ';'))
	{
		std::string trimmed = trim(token);
		if (!trimmed.empty()) {
			cookies.emplace_back(trimmed);
		}
	}
	return (cookies);
}
//Cookie-Specific HelperFncs --BEGIN

//Utils -- END


