#include "Connection.hpp"
#include "Server.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include <map>
#include <filesystem>
#include <sstream>

bool	Connection::is_redirect() {
	P_DEBUG("is_redirect\n");
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
			P_DEBUG("is_redirect pre 301\n");
			const std::string mod = request.uri->path.append("/");
			if (request.method.value() == "GET") //TODO: check with Steffen
				redirect(301,"http://" + it->second + mod);
			else if (request.method.value() == "DELETE") {
				P_DEBUG("In pre-exit: is_redirect\n");
				std::cout << "_expanded_path: " << _expanded_path << "\n";
				set_full_status_code(409);
			}
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
	P_DEBUG("malicious_request\n");
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
	// TODO this logic sucks - need improvement
	std::string currPath = std::filesystem::current_path();
	if (!currPath.empty() && currPath.back() == '/') {
		currPath.pop_back();
	}
	// std::filesystem::path path = _expanded_path;
	// if (is_directory(path) && path.is_absolute()) {
	// 	std::cout << coloring("is absolute", BLUE) << std::endl;
	// 	_absolute_path = _expanded_path;
	// } else if (!_expanded_path.empty() && _expanded_path.front() == '/') {
	// 	_absolute_path = currPath + _expanded_path;
	// } else {
	// 	_absolute_path = currPath + "/" + _expanded_path;
	// }
	_absolute_path = _expanded_path;

	std::cout << coloring("Absolute Path: " + _absolute_path, BLUE)  << std::endl;

	if (!method_is_allowed(_matching_route->getAllowedMethods())) return;

	std::cout << coloring("autoindex: " + (_matching_route->getAutoIndex() ? std::string("on") : std::string("off")), BLUE) << std::endl;
	_autoindex_enabled =_matching_route->getAutoIndex();

	std::cout << coloring(request.uri->path, BLUE) << std::endl;
	if (is_cgi(_absolute_path) && request.method.value() != "DELETE") {
		size_t pos = _absolute_path.rfind('.');
		std::string cgi_identifier;
		if (pos != std::string::npos) {
			cgi_identifier = _absolute_path.substr(pos, _absolute_path.length() -1);
		} else {
			cgi_identifier = _absolute_path;
		}
		std::cout << coloring("CGI identifier: " + cgi_identifier, BLUE) << std::endl;
		for (const auto& [fst, snd] :_matching_route->getCgi()) {
			// if (fst == ".py") {
			if (fst == cgi_identifier) {
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
		// std::cout << coloring("TESTETSETSET\n", BLUE) << std::endl;
        set_full_status_code(415); // unsupported media tpe
		return ;
	}
	methode_handler();
}

// Try to match Request URI with Location. If match -> expanding path
void	Connection::entry_process(void)
{
	//0:
	if (request.status_code.has_value()) {
		P_DEBUG("Early-Exit in entry_process\n");
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
		if (temp2.compare(0, temp1.size(), temp1) == 0) {
			if (location_names.size() > count) {
				longest_match = location_names;
				count = location_names.size();
			}
		}
		// if (temp2.find(temp1) != std::string::npos) {
		// 	if (location_names.size() > count) {
		// 		// longest_match = temp1;
		// 		longest_match = location_names;
		// 		count = location_names.size();
		// 	}
		// }
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
	return ;
}
