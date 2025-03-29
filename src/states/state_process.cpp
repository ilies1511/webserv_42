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

bool	Connection::is_redirect() {
	if (_matching_route->getRouteIsRedirect()) {
		std::cout << coloring("Location is a redirect", BLUE) << std::endl;
		auto [typeOfRedirect, newLocation] =_matching_route->getRedirect();
		redirect(typeOfRedirect, newLocation);
		return true;
	}
	if (std::filesystem::is_directory(_expanded_path)
		&& request.uri->path.back() != '/') {
		const auto& it = request.headers.find("host");
		if (it != request.headers.end()) {
			const std::string mod = request.uri->path.append("/");
			redirect(301, it->second + mod);
		} else {
			set_full_status_code(400); // INVALID REQUEST
		}
		return true;
	}
	return false;
}

bool	Connection::method_is_allowed(const std::vector<std::string>& methods) {
	// const std::vector<std::string>& methods =_matching_route->getAllowedMethods();
	auto it = std::find(methods.begin(), methods.end(), request.method.value());
	if (it != methods.end()) {
		std::cout << coloring("method: " + request.method.value() + " is allowed", BLUE) << std::endl;
		return true;
	} else {
		std::cout << coloring("method: " + request.method.value() + " is not allowed", BLUE) << std::endl;
		set_full_status_code(405);
		return false;
	}
}

bool	Connection::malicious_request() {
	if (_expanded_path.find("/..") != std::string::npos
		|| _expanded_path.find("/../") != std::string::npos
		|| _expanded_path.find("../") != std::string::npos) {
		set_full_status_code(403); // FORBIDDEN
		return true;
	}
	return false;
}

void	Connection::validate_match(std::string& longest_match) {

	std::cout << coloring("longest match " + longest_match, BLUE) << std::endl;

	_matching_route = _server._config.getLocation()[longest_match];
	if (longest_match.back() != '/') {
		longest_match.push_back('/');
	}
	// TODO check redirect !
	// if (is_redirect()) return;

	std::string remainder;
	if (request.uri->path.length() > longest_match.length()) {
		// remainder = request.uri->path.substr(longest_match.length() + 1, request.uri->path.length());
		remainder = request.uri->path.substr(longest_match.length() , request.uri->path.length());
		std::cout << coloring("remainder " + remainder, BLUE) << std::endl;
	}
	_expanded_path =_matching_route->getActualPath();
	std::cout << coloring("Actual Path: " + _expanded_path, BLUE) << std::endl;
	if (!remainder.empty()) {
		_expanded_path.append(remainder);
	}
	if (malicious_request()) return;

	if (is_redirect()) return;

	std::cout << coloring("Expanded Path: " + _expanded_path, BLUE) << std::endl;
	std::string currPath = std::filesystem::current_path();
	_absolute_path = currPath + "/" + _expanded_path;
	std::cout << coloring("Absolute Path: " + _absolute_path, BLUE)  << std::endl;

	if (!method_is_allowed(_matching_route->getAllowedMethods())) return;

	std::cout << coloring("autoindex: " + (_matching_route->getAutoIndex() ? std::string("on") : std::string("off")), BLUE) << std::endl;
	_autoindex_enabled =_matching_route->getAutoIndex();

	std::cout << coloring(request.uri->path, BLUE) << std::endl;
	if (is_cgi(_expanded_path)) {
		for (const auto& [fst, snd] :_matching_route->getCgi()) {
			if (fst == ".py") {
				const std::string cgiExec = snd; // TODO maybe check if cgi executable is valid
				std::cout << coloring("CGI executable: " + cgiExec, BLUE) << std::endl;
				_cgi.emplace();
				this->_state = State::CGI;
				this->_next_state = State::SEND;
				_cgi->setCgiEngine(snd);
				entry_cgi();
				return ;
			}
		}
        set_full_status_code(415); // unsupported media tpe
		return ;
	}
	std::cout << coloring("this shit should not get printed", BLUE) << std::endl;
	methode_handler();
}

// Try to match Request URI with Location. If match -> expanding path
void	Connection::entry_process(void)
{
	//0:
	if (request.status_code.has_value()) {
		set_full_status_code((size_t)*request.status_code);
		return;
	}
	std::cout << coloring("request uri_path: " + request.uri->path, BLUE) << std::endl;
	std::cout << coloring("request method: " + request.method.value(), BLUE) << std::endl;

	// getting URI of all locations
	const auto& all_locations = _server._config.getLocation();

	std::string longest_match;
	std::size_t count = 0;
	for (const auto& [location_names, route] : all_locations) {

		std::string temp2 = request.uri->path;
		if (temp2.back() != '/') {
			temp2.push_back('/');
		}
		std::string temp1 = location_names;
		if (temp1.back() != '/') {
			temp1.push_back('/');
		}
		if (temp2.find(temp1) != std::string::npos) {
			if (location_names.size() > count) {
				// longest_match = temp1;
				longest_match = location_names;
				count = location_names.size();
			}
		}
		// if (temp2.find(location_names) != std::string::npos) {
			// if (location_names.size() > count) {
				// longest_match = location_names;
				// count = location_names.size();
			// }
		// }
	}
	if (count == 0) {
		std::cout << coloring("NO MATCH!!!", BLUE) << std::endl;
        // if no location is matching and config doesn't have "location /" error will be generated because:
        // method and autoindex can only be activated inside a location
		set_full_status_code(403); // TODO check if forbidden or not found
		return;
	}
    validate_match(longest_match);

	/*
	TODO: PART 1
		0. check if request was invalid --> parser sets status-code xy : read file, send to clieant (assemble_resonse)
		1. Allgemeine Schritte:
			- location auswaehlen/matching
			- if location is a redirect
				--> just send 30x + specific Header
			- Path Variable zusammenbauen --> Config + Request = FilePath

			- Verifizierung ob Meth

		2. Check CGI:
			Conditional Blocks ((is_get || is_post() )&& is_cgi(_sys_path))
			{
				_state = State::CGI; //Kein write mehr notwending, da in CGI Mode gemacht wird
				_nex_state = SEND;
			}
			else {
				do normal cycle
			}
	*/

	/*
		PART 2:
			methode_handler()
	*/

	// request.method = "GET";
	// request.uri = "/index.html";
	// request._version = "HTTP/1.1";
	// request._body = "";
	// // ed_request.is_finished = true;
	// request.is_finished = true;
	// request.readFile = true;
	// // request.filename = "html/index.html";
	// // filled_request._headers = ;
	// std::cout << coloring(request.uri->path, BLUE) << std::endl;
	// if (is_cgi(request.uri->path)) {
	// 	this->_state = State::CGI;
	// 	this->_next_state = State::SEND;
	// 	return ;
	// }
	// methode_handler();
	return ;
}
