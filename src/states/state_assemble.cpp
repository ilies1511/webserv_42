#include "Connection.hpp"
#include "Server.hpp"
#include "HTTP_Parser.hpp"
// #include "Request.hpp"
#include "RequestParser.hpp"
#include "Response.hpp"
#include "printer.hpp"
#include "StaticFileHandler.hpp"
#include <map>

void Connection::assemble_response()
{
	std::string reason = _current_response.reason_phrase;

	_current_response.http_version = "HTTP/1.1";
	_current_response.headers["Connection"] = "close";
	// _current_response.headers["Content-Type"] = "text/html";
	auto it = _current_response.headers.find("Content-Type");
	std::string tmp = "mime_type: " + \
		_current_response.headers["Content-Type"] +"\n";
	P_DEBUG(tmp.c_str());
	if (it == _current_response.headers.end()) {
		std::string tmp = "mime_type: " + \
							 _current_response.headers["Content-Type"] +"\n";
		P_DEBUG(tmp.c_str());
		_current_response.headers["Content-Type"] = get_mime_type(_system_path);
	}
	_current_response.headers["Content-Length"] = \
		std::to_string(this->_current_response.file_data.size());

	//Add hardcoded Headers
	// if (_current_response.status_code != "500" || _current_response.status_code != "403") // condition nur temporaer da um zu 500 Case zu testen
	// {
	// 	std::cout << coloring("\nin assemble_response()\n", TURQUOISE);
	// 	_current_response.http_version = "HTTP/1.1";
	// 	_current_response.status_code = "200";
	// 	_current_response.headers["Connection"] = "close";
	// 	_current_response.headers["Content-Type"] = "text/html";
	// 	_current_response.headers["Content-Length"] = \
	// 	std::to_string(this->_current_response.file_data.size());
	// }
	if (reason.empty()) {
		const std::map<std::string, std::string> default_status_texts = {
			{"200", "OK"},
			{"301", "Moved Permanently"},
			{"400", "Bad Request"},
			{"403", "Forbidden"},
			{"404", "Not Found"},
			{"500", "Internal Server Error"}
		};
		auto it = default_status_texts.find(_current_response.status_code);
		if (it != default_status_texts.end()) {
			reason = it->second;
		}
	}
	// Assemble status line (HTTP-Version, Statuscode, Reason-Phrase)
	std::string assembled_response =	_current_response.http_version + " " +
										_current_response.status_code + " " +
										reason + "\r\n";
	// Alle Header durchlaufen und hinzufügen
	for (const auto &header : _current_response.headers) {
		assembled_response += header.first + ": " + header.second + "\r\n";
	}
	//Add line
	assembled_response += "\r\n";
	//Append file_data
	assembled_response += _current_response.file_data;
	// FF - FInohed Fusion
	_current_response.response_inzemaking = assembled_response;
	//Change State to send, since by now, the reponse should be done here
	_state = State::SEND;
}
