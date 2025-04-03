#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

void	Connection::generate_autoindex(const std::filesystem::path& dir)
{
	std::ostringstream body;
	body << "<html><body><h1>Index of " << dir.filename() << "</h1><ul>";

	for (const auto& entry : std::filesystem::directory_iterator(dir))
	{
		std::string name = entry.path().filename().string();
		body << "<li><a href=\"" << name << "\">" << name << "</a></li>";
	}
	body << "</ul></body></html>";

	_current_response.status_code = "200";
	_current_response.reason_phrase = "OK";
	_current_response.http_version = "HTTP/1.1";
	_current_response.file_data = body.str();
	_current_response.headers["Content-Type"] = "text/html";
	_current_response.headers["Content-Length"] = std::to_string(_current_response.file_data.size());
	_current_response.headers["Connection"] = "close";
}

bool	Connection::file_exists_and_readable(const std::filesystem::path& p)
{
	return (std::filesystem::exists(p) \
			&& std::filesystem::is_regular_file(p) \
			&& access(p.c_str(), R_OK) == 0);
}

/*
	filled_request._method = "GET";
	filled_request._uri = "/index.html";
	filled_request._version = "HTTP/1.1";
	filled_request._body = "";
	// filled_request.is_finished = true;
	filled_request.is_finished = true;
	filled_request.readFile = true;
	filled_request.filename = "html/index.html";
*/

void	Connection::redirect(size_t input_status_code, std::string New_Location)
{
	this->_current_response.status_code = std::to_string(input_status_code);
	_current_response.headers["Location"] = New_Location;
	_current_response.headers["Connection"] = "close";
	_system_path = _server._config.getErrorPages()[input_status_code];
	// for (auto v : _server._config.getErrorPages()) {
	// 	std::cout << v.second << std::endl;
	// }
	prepare_fdFile_param(std::to_string(input_status_code));
}

/*
	http://localhost:9035/var/www/data/files
*/
void	Connection::no_trailing_slash_case(void)
{
	if (std::filesystem::exists(_full_path))
	{
		if (std::filesystem::is_regular_file(_full_path))
		{
			std::cout << "\n\nALO LO\n\n";
			// prepare_fdFile_param("200");
			set_full_status_code(200, _full_path);
		}
		else if (std::filesystem::is_directory(_full_path))
		{
			_current_response.headers["Content-Type"] =  "text/html";
			std::string corrected_uri = request.uri->path;
			if (!corrected_uri.empty() && corrected_uri.back() != '/')
				corrected_uri += '/';
			redirect(301, corrected_uri);
			return ;
		}
	}
	else
	{
		std::cout << "Asehr\n";
		// for (auto v : _server._config.getErrorPages()) {
		// 	std::cout << v.second << std::endl;
		// }
		set_full_status_code(404);
		return ;
	}
}

/*
	http://localhost:9035/var/www/data/files/
*/
void	Connection::trailing_slash_case(void)
{
	std::cout << coloring("In has_trailing_slash()\n", TURQUOISE);
	if (!std::filesystem::exists(_full_path))//Case tritt auf, wenn path nicht existiert
	{
		std::cout << "CR7\n";
		set_full_status_code(404);
		return ;
	}
	std::filesystem::path index_file = _full_path / "index.html"; // ==> "/Users/iziane/42/repo_webserv/webserv/var/www/data/files/index.html
	if (file_exists_and_readable(index_file)) {
		printer::debug_putstr("file_exists_and_readable(index_file) CASE", __FILE__, __FUNCTION__, __LINE__);
		_system_path = index_file;
		std::cout << "im here\n";
		set_full_status_code(200, index_file);
		return ;
	}
	else if (_autoindex_enabled) {
		generate_autoindex(_full_path);
		_state = State::ASSEMBLE;
		return ;
	}
	else
	{
		printer::debug_putstr("403 CASE", __FILE__, __FUNCTION__, __LINE__);
		set_full_status_code(403);
		return ;
	}
}

/*
	// TODO: 22.03.2025 status_code - *.html Mapping --> dynamic and no hardcode
	// this->request._uri = "var/www/data/files/trier";
	// this->request._uri = "var/www/data/files"; // No slash case
	// this->request.uri = "var/www/data/files"; // No slash case

	// std::filesystem::path full_path = "/Users/iziane/42/repo_webserv/webserv/" / this->request.uri;
	// std::filesystem::path full_path = "/Users/iziane/42/repo_webserv/webserv/" + this->request.uri;
	// std::filesystem::path full_path = "/Users/iziane/42/bserv/webserv/" + this->request.uri;
*/
void	Connection::handle_get(void)
{
	std::cout << "\nALLOOOO aus handle_get entry\n";

	_full_path = _absolute_path;
	_full_path = std::filesystem::weakly_canonical(_full_path);
	std::cout << "\nPOST weakly_canonical - _full_path: " << _full_path << "\n";
	this->_system_path = _full_path;

	bool has_trailing_slash = !request.uri->path.empty() && request.uri->path.back() == '/';
	if (has_trailing_slash) {//nginx assumes this is dir
		trailing_slash_case();
	}
	else { // nginx assumes this is file
		no_trailing_slash_case();
	}
}
