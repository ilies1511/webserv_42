#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
// #include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
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
	assert(pfd);

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

// void	Connection::print_request_data(Request &request)
// {
// 	std::cout << " Method: " \
// 					<< request._method << " URI:" \
// 					<< request._uri \
// 					<< " Version: " << request._version \
// 					<< " Finished: " << request.is_finished \
// 					<< "\n";
// }

void	Connection::prepare_fdFile_param(const std::string status_code)
{
	_current_response.status_code = status_code;

	// _system_path = "errorPages/403.html";
	std::cout << coloring("\n\nin prepare_fdFile - _system_path: " + _system_path + "\n", TURQUOISE);
	_fdFile = open(_system_path.c_str(), O_RDONLY | O_NONBLOCK);
	if (_fdFile < 0)
	{
		//std::cout << _system_path << std::endl;
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
	// volatile int *p = NULL;

	// *p = 123;
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
	// key = static_cast<size_t>(std::stoull(status_code));
	// _system_path = _server._config.getErrorPages()[key];
	// _system_path = _server._config.getErrorPages()[status];
	// prepare_fdFile_param(std::to_string(status));
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
		{".html", "text/html"},
		{".txt",  "text/plain"},
		{".css",  "text/css"},
		{".js",   "application/javascript"},
		{".png",  "image/png"},
		{".jpg",  "image/jpeg"},
		// Videoformate
		{".mp4",  "video/mp4"},
		{".webm", "video/webm"},
		{".ogg",  "video/ogg"},
		{".mov",  "video/quicktime"},
		{".ico",  "image/vnd.microsoft.icon"},
		{".avi",  "video/x-msvideo"}
	};

	auto it = mimeTypes.find(ext);
	if (it != mimeTypes.end()) {
		return it->second;
	}
	// Default: entweder text/plain oder application/octet-stream
	return "application/octet-stream";
}

//Cookie-Specific HelperFncs --BEGIN
void	Connection::check_cgi_response_for_cookies(void)
{

	std::string output = _cgi->getOutput();
	std::cout << coloring(output, BLUE) << std::endl;
	std::regex pattern(R"(Set-Cookie:?\s*([^\r\n]*))", std::regex::icase);
	auto begin = std::sregex_iterator(output.begin(), output.end(), pattern);
	auto end = std::sregex_iterator();

	for (std::sregex_iterator i = begin; i != end; ++i) {
		std::smatch match = *i;
		if (match.size() > 1) {
			std::string str = match[1].str();
			if (str.back() == '\n' || str.back() == '\r') {
				str.pop_back();
			}
			if (str.find(';') != std::string::npos) {
				std::vector<std::string> arr;
				std::stringstream ss(str);
				std::string cookie_name;

				if (!std::getline(ss, cookie_name, ';')) {
					continue;;
				}
				Cookie cookie(cookie_name);
				std::string temp;
				while (std::getline(ss, temp, ';')) {
					if (temp.compare(0, 8, "Max-Age=")) {
						std::string stime = temp.substr(9);
						try {
							long max_age = std::stol(stime);
							std::time_t now = std::time(nullptr);
							now += max_age;
							cookie.setMaxAge(now);
						} catch (...) {
							continue;
						}
					}
					arr.push_back(temp);
				}
				std::cout << "adding cookie 1\n";
				bool found = false;
				for (auto& coo : _server._cookies) {
					std::cout << coloring("str: " + cookie_name + "; cookie1: " + coo.cookie_string + " " + std::to_string(coo.getMaxAge()), BLUE) << std::endl;
					if (coo.cookie_string == cookie_name) {
						found = true;
						if (coo.getMaxAge() != 0) {
							coo.max_age = cookie.max_age;
						}
					}
				}
				if (!found) {
                    _server._cookies.emplace_back(cookie);
				}
			} else {
				Cookie cookie(str);
				std::cout << "adding cookie 2\n";
				bool found = false;
				for (auto& coo : _server._cookies) {
					std::cout << coloring("cookie2: " + coo.cookie_string + " " + std::to_string(coo.getMaxAge()), BLUE) << std::endl;
					if (coo.cookie_string == str) {
						found = true;
						if (coo.getMaxAge() != 0) {
							coo.max_age = cookie.max_age;
						}
					}
				}
				if (!found) {
					_server._cookies.emplace_back(cookie);
				}
			}
		}
	}
	std::cout << coloring("TEST", BLUE) << std::endl;
	// std::cout << coloring("COOKIE: "+ _cgi->getOutput(),BLUE) << std::endl;
	// for (;;)
	// {
	// 	break ;
	// }
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
			// cookies.push_back("");
			cookies.emplace_back(trimmed);
		}
	}
	return (cookies);
}
//Cookie-Specific HelperFncs --BEGIN

//Utils -- END


