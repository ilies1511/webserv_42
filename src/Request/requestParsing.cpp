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
 *	check HTTP Version / if not supportet 505 HTTP VERSION NOT SUPPORTED
 */

void requestParser::parseRequestLine() {
	std::cout << "line: " << _currentLine << std::endl;
	const std::vector<std::string> slices = split();
	if (slices.size() != 3) {
		throw std::runtime_error("400 Bad Request");
	}
	_requestLine.method = slices[0];
	_requestLine.uri = slices[1];
	_requestLine.version = slices[2];

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