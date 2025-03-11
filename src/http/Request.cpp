#include "Request.hpp"

Request::Request(void)
	: _method{}, _uri{}, _version{}, _headers{}, _body{}
{}

Request::~Request(void) {}

Request::Request(const Request& other)
{
	*this = other;
}

Request& Request::operator=(const Request& other)
{
	if (this != &other)
	{
		this->_method = other._method;
		this->_uri = other._uri;
		this->_version = other._version;
		this->_headers = other._headers;
		this->_body = other._body;
	}
	return (*this);
}
