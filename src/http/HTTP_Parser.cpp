#include "HTTP_Parser.hpp"

HTTP_Parser::HTTP_Parser(void) {}
HTTP_Parser::~HTTP_Parser(void) {}

bool	HTTP_Parser::parse_header(Buffer &InputBuffer, Request &request)
{
	(void)InputBuffer;
	(void)request;
	return (true);
}

bool	HTTP_Parser::parse_body(Buffer &InputBuffer, Request &request)
{
	(void)InputBuffer;
	(void)request;
	return (true);
}
