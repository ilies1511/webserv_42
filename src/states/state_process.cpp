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

void	Connection::entry_process(void)
{
	//0:
	if (request.status_code.has_value())
	{
		set_full_status_code((size_t)*request.status_code);
	}
	std::cout << coloring("Entry Process Test", BLUE) << std::endl;
	std::cout << coloring("system path: " + _system_path, BLUE) << std::endl;
	std::cout << coloring("request uri_full: " + request.uri->full, BLUE) << std::endl;
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
		if (temp2.find(location_names) != std::string::npos) {
			if (location_names.size() > count) {
				longest_match = location_names;
				count = location_names.size();
			}
		}
	}
	if (count == 0) {
		std::cout << coloring("NO MATCH!!!", BLUE) << std::endl;
	} else {
        std::cout << coloring("longest match " + longest_match, BLUE) << std::endl;

		// matching location
		const route matchingRoute =  _server._config.getLocation()[longest_match];
		// todo check redirect !
		if (matchingRoute.getRouteIsRedirect()) {
			std::cout << coloring("Location is a redirect", BLUE) << std::endl;
			// TODO add redirect
		}

		std::string remainder;
		if (request.uri->path.length() > longest_match.length()) {
			remainder = request.uri->path.substr(longest_match.length() + 1, request.uri->path.length());
			std::cout << coloring("remainder " + remainder, BLUE) << std::endl;
		}
		_expanded_path = matchingRoute.getActualPath();
		std::cout << coloring("Actual Path: " + _expanded_path, BLUE) << std::endl;
		if (!remainder.empty()) {
			_expanded_path.append(remainder);
		}
		std::cout << coloring("Expanded Path: " + _expanded_path, BLUE) << std::endl;
		std::string currPath = std::filesystem::current_path();
		std::cout << coloring("Absolute Path: " + currPath + "/" + _expanded_path,BLUE) << std::endl;

		// const route matchingRoute =  _server._config.getLocation()[longest_match];
		std::vector<std::string> methods = matchingRoute.getAllowedMethods();
		auto it = std::find(methods.begin(), methods.end(), request.method.value());
		if (it != methods.end()) {
			std::cout << coloring("method: " + request.method.value() + " is allowed", BLUE) << std::endl;
		} else {
			std::cout << coloring("method: " + request.method.value() + " is not allowed", BLUE) << std::endl;
		}
		std::cout << coloring("autoindex: " + (matchingRoute.getAutoIndex() ? std::string("on") : std::string("off")), BLUE) << std::endl;
	}
	// if no location is matching and config doesn't have "location /" error will be generated because:
	// method and autoindex can only be activated inside a location


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
	if (is_cgi(request.uri->path)) {
		this->_state = State::CGI;
		this->_next_state = State::SEND;
		return ;
	}
	methode_handler();
	return ;
}
