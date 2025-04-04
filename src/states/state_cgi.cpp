#include "Connection.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

//TODO: just a placeholder for now

bool Connection::is_cgi(std::string &path) {
	if (request.status_code.has_value())
	{
		return (false);
	}
	if (path.length() > 3) {
		size_t dot_pos = path.find_last_of('.');
		if (dot_pos != std::string::npos) {
			std::string ext = path.substr(dot_pos);
			return (ext == ".py" || ext == ".pl" ||ext == ".php");
		}
	}
	return false;
}

void	Connection::setup_cgi() {
	// setValues();
	// if (!std::filesystem::exists(_expanded_path) || !std::filesystem::is_regular_file(_expanded_path)) {
	// 	set_full_status_code(404);
	// 	_cgi->setCgiState(FINISH);
	// 	return;
	// }
	// if (access(_expanded_path.data(), X_OK)) {
	// 	std::cout << coloring("CGI TEST 3", BLUE) << std::endl;
	// 	set_full_status_code(403);
	// 	_cgi->setCgiState(FINISH);
	// 	return;
	// }
	if (!std::filesystem::exists(_absolute_path) || !std::filesystem::is_regular_file(_absolute_path)) {
		set_full_status_code(404);
		_cgi->setCgiState(FINISH);
		return;
	}
	if (access(_absolute_path.data(), X_OK)) {
		std::cout << coloring("CGI PERMISSION DENIED", BLUE) << std::endl;
		set_full_status_code(403);
		_cgi->setCgiState(FINISH);
		return;
	}

	_cgi->setMethod(request.method.value());
	// if (request.method.value() == "GET") {
	// 	_cgi->setOutput("HTTP/1.1 200 OK\r\n");
	// } else if (request.method.value() == "POST") {
	// 	_cgi->setOutput("HTTP/1.1 201 CREATED\r\n");
	// }
	_cgi->setScript(_absolute_path);
	// _cgi->setQueryString(request.uri->query);
	// _cgi->setCgiEngine(cgiEngine);
	// _cgi->setContentLength(std::to_string(request.body.value().length()));
	// auto it = request.headers.find("content-type");
	// if (it != request.headers.end()) {
	// 	_cgi->setContentType(it->second);
	// }
	// std::vector<std::string> env;

	// //TODO: 02.04.25
	// if (request.headers.find("cookie") != request.headers.end()) {
	// }

	auto cookie_finder = request.headers.find("cookie");
	if (cookie_finder != request.headers.end()) {
		std::vector<std::string> res_split = cookie_split(request.headers["cookie"]);
		std::cout << res_split.size() << "== size\n";
		for (const auto& cookie : res_split) {
			if (_server.is_valid_cookie(cookie)) { // TODO: 03.04.25 Annahme: Server-Validierung
				std::cout << "res_split: " << cookie << "\n";
				_cgi->_env.emplace_back(cookie);
			}
		}

		//TODO: split following string --> split bases on ';' delimiter
		/*
			std::string cookies = "cookies: key1=val1; key2=val2";
			_cgi->_env.emplace_back("key1=val1");
			_cgi->_env.emplace_back("key2=val2");

			1. split oder substring (den Cookies Header aus dem Response)

			2. Checken ob z.B folgender String in der Datenstruktur(pro Server)
				ist: "1=1" --> wird erstellt wenn CGI fertig ist
				check with: bool Server::is_valid_cookie(const std::string& cookie_string)
			3. CGI fuegt es bei erfolgreicher Validierung hinzu:
				gemaeß : _cgi->_env.emplace_back("key2=val2"); (vgl. mit env)
				_cgi->_env.emplace_back("1=1");

				request.headers["cookie"]  == "1=1; 2=321"
		*/
	}
	_cgi->_env.emplace_back("REQUEST_METHOD=" + request.method.value());
    _cgi->_env.emplace_back("SCRIPT_NAME=" + _absolute_path);
	_cgi->_env.emplace_back("PATH_INFO=" + _absolute_path); // TODO add actual path info
	_cgi->_env.emplace_back("QUERY_STRING=" + request.uri->query);
	auto it = request.headers.find("content-type");
	if (it != request.headers.end()) {
		_cgi->_env.emplace_back("CONTENT_TYPE=" + it->second);
		// _cgi->setContentType(it->second);
	} else {
		_cgi->_env.emplace_back("CONTENT_TYPE=");
	}
	if (request.body.has_value()) {
		_cgi->_env.emplace_back("CONTENT_LENGTH=" + std::to_string(request.body.value().length()));
		_cgi->setBody(request.body.value());
	} else {
		_cgi->_env.emplace_back("CONTENT_LENGTH=");
	}
	for (const auto& [fst, snd] : request.headers) {
		std::string key = "HTTP_" + fst;
		std::replace(key.begin(), key.end(), '-', '_');
		std::transform(key.begin(), key.end(), key.begin() ,::toupper);
		_cgi->_env.emplace_back(key + "=" += snd);
	}

	// std::cout << coloring("CGI CGI CGI CGI CGI CGI CGI CGI CGI CGI CGI CGI CGI CGI", BLUE) << std::endl;;
	for (auto& environment : _cgi->_env) {
		// std::cout << coloring(environment, BLUE) << std::endl;;
		_cgi->setEnvp(environment);
	}
	_cgi->setup_connection(*this);
}

void	Connection::entry_cgi()
{
	switch (_cgi->getCgiState()) {
		case INIT:
			setup_cgi();
			break;
		case WRITE:
			_cgi->writing(*this);
			break;
		case WAIT:
			_cgi->waiting(*this);
			break;
		case READ:
			_cgi->readCgiOutput(*this);
			break;
		case FINISH:
			_current_response.response_inzemaking = _cgi->parseCgiOutput();
			_state = State::SEND;
			check_cgi_response_for_cookies();
			break;
		case ERROR:
			set_full_status_code(500);
			break;
	}
	// access all necessary data
	// if (_cgi->getCgiState() == INIT) {
	// 	// std::cout << coloring("CGI INIT", BLUE) << std::endl;
	// 	setup_cgi();
	// 	// _cgi->setup_connection();
	// 	// std::cout << coloring("CGI INIT FINISHED", BLUE) << std::endl;
	// 	// std::cout << coloring("NEXT STATE: " + std::to_string(_cgi->getCgiState()), BLUE) << std::endl;;
	// }
	// else if (_cgi->getCgiState() == WRITE) {
	// 	// std::cout << coloring("CGI WRITING", BLUE) << std::endl;
	// 	_cgi->writing();
	// 	// std::cout << coloring("CGI WRITING FINISHED", BLUE) << std::endl;
	// }
 //    else if (_cgi->getCgiState() == WAIT) {
	// 	// std::cout << coloring("CGI WAITING", BLUE) << std::endl;
	// 	_cgi->waiting();
	// 	// std::cout << coloring("CGI WAITING FINISHED", BLUE) << std::endl;
 //    }
	// else if (_cgi->getCgiState() == READ) {
	// 	// std::cout << coloring("CGI READ", BLUE) << std::endl;
	// 	_cgi->readCgiOutput();
	// 	// std::cout << coloring("CGI READ FINISHED", BLUE) << std::endl;
	// }
	// else if (_cgi->getCgiState() == FINISH) {
	// 	std::cout << coloring("CGI FINISH CHECK", BLUE) << std::endl;
	// 	this->_state = State::SEND;
	// 	// TODO HARDCODED ERROR to avoid endless loop
	// 	// set_full_status_code(404);
	// 	// return;
	// 	return;
	// } else if (_cgi->getCgiState() == ERROR) {
	// 	set_full_status_code(500);
	// }
	// TODO implement CGI error handling
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
