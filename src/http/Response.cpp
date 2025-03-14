#include "Response.hpp"
#include "Request.hpp"

// Response::Response(void)
// 	: status_code{}, body{}, headers{}, _data()
// {}

Response::Response(const Request &request)
	:	status_code{},
		body{},
		headers{},
		FileData(false),
		file_data(""),
		response_inzemaking(""),
		_data(request)
{}

Response::Response(const Response& other)
	: status_code(other.status_code), body(other.body), headers(other.headers), _data(other._data)
{}

Response::~Response(void) {}

Response& Response::operator=(const Response& other)
{
	if (this != &other)
	{
		this->status_code = other.status_code;
		this->body = other.body;
		this->headers = other.headers;
		// this->_data = other._data;
	}
	return (*this);
}
