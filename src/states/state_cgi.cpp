#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "printer.hpp"
// #include "StaticFileHandler.hpp"
#include <map>
#include <filesystem>
#include <sstream>

//TODO: just a placeholder for now
bool Connection::is_cgi(std::string &path) {
	if (request.status_code.has_value())
	{
		return (false);
	}
	// if (path.length() > 3
	// 	&& path.find_last_of(".py") == (size_t)(path.end() - path.begin() - 3)
	// 	&& (request.method == "GET" || request.method == "POST"))
	// {
	// 	return (true);
	// }
	if (path.length() > 3 && path.find_last_of(".py") == path.length() - 1) {
        return true;
	}
	return (false);
}

void	Connection::setup_cgi() {
	// setValues();
	_cgi->setMethod(request.method.value());
	_cgi->setScript(_expanded_path);
	// _cgi->setQueryString(request.uri->query);
	// _cgi->setCgiEngine(cgiEngine);
	// _cgi->setContentLength(std::to_string(request.body.value().length()));
	// auto it = request.headers.find("content-type");
	// if (it != request.headers.end()) {
	// 	_cgi->setContentType(it->second);
	// }
	std::vector<std::string> env;
	env.emplace_back("REQUEST_METHOD=" + request.method.value());
	env.emplace_back("SCRIPT_NAME=" + _expanded_path);
	env.emplace_back("PATH_INFO=" + _expanded_path); // TODO add actual path info
	env.emplace_back("QUERY_STRING=" + request.uri->query);
	auto it = request.headers.find("content-type");
	if (it != request.headers.end()) {
		env.emplace_back("CONTENT_TYPE=" + it->second);
		// _cgi->setContentType(it->second);
	} else {
		env.emplace_back("CONTENT_TYPE=");
	}
	if (request.body.has_value()) {
		env.emplace_back("CONTENT_LENGTH=" + std::to_string(request.body.value().length()));
	} else {
		env.emplace_back("CONTENT_LENGTH=");
	}
	for (const auto& [fst, snd] : request.headers) {
		std::string key = "HTTP_" + fst;
		std::replace(key.begin(), key.end(), '-', '_');
		std::transform(key.begin(), key.end(), key.begin() ,::toupper);
		env.emplace_back(key + "=" += snd);
	}

	for (auto environment : env) {
		_cgi->setEnvp(environment);
	}
}

void	Connection::entry_cgi()
{
	// access all necessary data
	if (_cgi->getCgiState() == INIT) {
		std::cout << coloring("CGI INIT", BLUE) << std::endl;
		setup_cgi();
		_cgi->setup_connection();
		std::cout << coloring("CGI INIT FINISHED", BLUE) << std::endl;
		std::cout << coloring("NEXT STATE: " + std::to_string(_cgi->getCgiState()), BLUE) << std::endl;;
	}
	else if (_cgi->getCgiState() == WRITE) {
		std::cout << coloring("CGI WRITING", BLUE) << std::endl;
		_cgi->writing();
		std::cout << coloring("CGI WRITING FINISHED", BLUE) << std::endl;
	}
    else if (_cgi->getCgiState() == WAIT) {
		std::cout << coloring("CGI WAITING", BLUE) << std::endl;
		_cgi->waiting();
		std::cout << coloring("CGI WAITING FINISHED", BLUE) << std::endl;
    }
	else if (_cgi->getCgiState() == READ) {
		std::cout << coloring("CGI READ", BLUE) << std::endl;
		_cgi->readCgiOutput();
		std::cout << coloring("CGI READ FINISHED", BLUE) << std::endl;
	}
	else if (_cgi->getCgiState() == FINISH) {
		std::cout << coloring("CGI FINISH CHECK", BLUE) << std::endl;
		this->_state = State::SEND;
		return;
	}

	/*
	if (run_cgi is finished)
	{
		if (!(cgi had error))
		{
			_state = _next_state;
		}
		else
		{
			set_full_status_code(cgi_status_code);
			return ;
		}
	}
	*/
}
