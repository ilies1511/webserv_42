#include "../../Includes/Request/requestParsing.hpp"
//#include "webserv.hpp"
#include <iostream>
#include <utility>

requestParser::requestParser(std::string  requestStr, const std::vector<serverConfig>& configs) : _serverConfigs(configs),
																					_startPos(0),
																					_requestStr(std::move(requestStr)) {
	_endPos = _requestStr.find("\r\n");
};

requestParser::~requestParser() {};

std::vector<std::string> requestParser::split() {
	std::vector<std::string> slices;
	std::size_t start = 0;
	std::size_t end = _currentLine.find(' ');

	while (end != std::string::npos) {
		slices.push_back(_currentLine.substr(start, end - start));
		start = end + 1;
		end = _currentLine.find(' ', start);
	}
	slices.push_back(_currentLine.substr(start, end - start));
	return slices;

}

/*
 * Request line validation:
 *
 * PATH = root + route
 * ROUTE data OVERWRITE SERVER data -> so always check ROUTE FIRST, if there is no data check server
 *
 *	check if PATH is in CONFIGFILE and EXISTS
 *	check for static file locations
 *	check METHODS and PERMISSION to EXECUTE them
 *
 *	check HTTP Version / if not supported 505 HTTP VERSION NOT SUPPORTED
 */

bool requestParser::hasReadPermission(const std::string &path) {
	std::filesystem::perms p = std::filesystem::status(path).permissions();

	const bool owner = (p& std::filesystem::perms::owner_read) != std::filesystem::perms::none;
	const bool group = (p& std::filesystem::perms::group_read) != std::filesystem::perms::none;
	const bool other = (p& std::filesystem::perms::others_read) != std::filesystem::perms::none;

	return owner || group || other;
}


void requestParser::parseRequestLine() {
	std::cout << "line: " << _currentLine << std::endl;
	const std::vector<std::string> slices = split();
	if (slices.size() != 3) {
		throw std::runtime_error("400 Bad Request");
	}
	_requestLine.method = slices[0];
	_requestLine.uri = slices[1];
	_requestLine.version = slices[2];

	std::string fileCheck = _serverConfigs[0].getRoot() + _requestLine.uri;
	if (std::filesystem::exists(fileCheck)) {
		if (std::filesystem::is_regular_file(fileCheck)) {
			// dont need to check method
			std::cout << fileCheck << " is a file ";
			if (hasReadPermission(fileCheck)) {
				std::cout << "and has read permission" << std::endl;
			} else {
				std::cout << "and has NO read permission" << std::endl;
				throw std::runtime_error("403 Forbidden");
			}
		}
	}
	try {
		std::string path;
		if (_serverConfigs[0].getRoute(_requestLine.uri).getRoot().empty()) {
			path = _serverConfigs[0].getRoot() + _requestLine.uri;
			if (std::filesystem::exists(path)) {
				std::cout << path << " is a directory " << std::endl;
				if (hasReadPermission(path)) {
					std::cout << "and has read permission" << std::endl;
				} else {
					std::cout << "and has No read permission" << std::endl;
					throw std::runtime_error("403 Forbidden");
				}
			} else {
				throw std::runtime_error("404 Not Found");
			}
		} else {
			path = _serverConfigs[0].getRoute(_requestLine.uri).getRoot() + _requestLine.uri;
			if (std::filesystem::exists(path)) {
				std::cout << path << " is a directory " << std::endl;
				if (hasReadPermission(path)) {
					std::cout << "and has read permission" << std::endl;
				} else {
					std::cout << "and has No read permission" << std::endl;
					throw std::runtime_error("403 Forbidden");
				}
			} else {
				throw std::runtime_error("404 Not Found");
			}
		}
	} catch (const std::exception& e) {
		throw;
	}
};

void requestParser::parseRequestFields(const std::string& line) {
	std::cout << "field: " << line << std::endl;
}

void requestParser::parseHeader() {
	if (_endPos != std::string::npos) {
		_currentLine = _requestStr.substr(_startPos, _endPos - _startPos);
		parseRequestLine();
		_startPos = _endPos + 2;
	}
	while ((_endPos = _requestStr.find("\r\n", _startPos)) != std::string::npos) {
		parseRequestFields(_requestStr.substr(_startPos, _endPos - _startPos));
		_startPos = _endPos + 2;
	}
}

void requestParser::parseMessage() {
	parseHeader();

};