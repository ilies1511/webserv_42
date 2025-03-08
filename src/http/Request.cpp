#include "Request.hpp"

Request::Request(void)
	: _method{}, _uri{}, _version{}, _headers{}, _body{}
{}
Request::~Request(void) {}
