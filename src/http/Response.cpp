#include "Response.hpp"
#include "RequestParser.hpp"

Response::Response(void)
	:	http_version(""),
		status_code(""),
		reason_phrase(""),
		headers{},
		body(""),
		response_inzemaking(""),
		FileData(false),
		file_data(""),
		ready2send(false)
{}

Response::Response(const Response& other)
	:	http_version(other.http_version),
		status_code(other.status_code),
		reason_phrase(other.reason_phrase),
		headers(other.headers),
		body(other.body),
		response_inzemaking(other.response_inzemaking),
		FileData(other.FileData),
		file_data(other.file_data),
		ready2send(other.ready2send)
{}

Response::~Response(void) {}

Response& Response::operator=(const Response& other)
{
	if (this != &other)
	{
		http_version = other.http_version;
		status_code = other.status_code;
		reason_phrase = other.reason_phrase;
		headers = other.headers;
		body = other.body;
		response_inzemaking = other.response_inzemaking;
		FileData = other.FileData;
		file_data = other.file_data;
		ready2send = other.ready2send;
	}
	return (*this);
}

