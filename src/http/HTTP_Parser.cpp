// #include "HTTP_Parser.hpp"

// Parser::Parser(void) {}
// Parser::~Parser(void) {}

// bool	Parser::request_line(Buffer &InputBuffer, Request &request)
// {
// 	(void)InputBuffer;
// 	(void)request;
// 	return (true);
// }

// bool	Parser::parse_header(Buffer &InputBuffer, Request &request)
// {
// 	(void)InputBuffer;
// 	(void)request;
// 	return (true);
// }

// bool	Parser::parse_body(Buffer &InputBuffer, Request &request)
// {
// 	(void)InputBuffer;
// 	(void)request;
// 	return (true);
// }

// Request Parser::entry_parse(Buffer &InputBuffer, Request &filled_request)
// {
// 	(void)InputBuffer;

// 	// Request	filled_request;

// 	if (this->request_line(InputBuffer, filled_request))
// 	{
// 		if (this->parse_header(InputBuffer, filled_request))
// 		{
// 			if (this->parse_body(InputBuffer, filled_request))
// 			{
// 				_parsing_finished = true;
// 			}
// 			else
// 			{
// 				/*
// 					TODO:Errro-Case for failed parse_body();
// 				*/
// 				;
// 			}
// 		}
// 		else
// 		{
// 			/*
// 				TODO:Errro-Case for failed parse_header();
// 			*/
// 			;
// 		}
// 	}
// 	else {
// 		/*
// 			TODO:Errro-Case for failed request_line();
// 		*/
// 	}
// 	filled_request._method = "GET";
// 	filled_request._uri = "/index.html";
// 	filled_request._version = "HTTP/1.1";
// 	filled_request._body = "";
// 	// filled_request.is_finished = true;
// 	filled_request.is_finished = true;
// 	filled_request.readFile = true;
// 	filled_request.filename = "html/index.html";
// 	// filled_request._headers = ;
// 	return (filled_request);
// }


// // Request Parser::entry_parse(Buffer &InputBuffer)
// // {
// // 	(void)InputBuffer;

// // 	Request	filled_request;

// // 	if (this->request_line(InputBuffer, filled_request))
// // 	{
// // 		if (this->parse_header(InputBuffer, filled_request))
// // 		{
// // 			if (this->parse_body(InputBuffer, filled_request))
// // 			{
// // 				_parsing_finished = true;
// // 			}
// // 			else
// // 			{
// // 				/*
// // 					TODO:Errro-Case for failed parse_body();
// // 				*/
// // 				;
// // 			}
// // 		}
// // 		else
// // 		{
// // 			/*
// // 				TODO:Errro-Case for failed parse_header();
// // 			*/
// // 			;
// // 		}
// // 	}
// // 	else {
// // 		/*
// // 			TODO:Errro-Case for failed request_line();
// // 		*/
// // 	}
// // 	filled_request._method = "Aloooo";
// // 	filled_request._uri = "/index.html";
// // 	filled_request._version = "HTTP/1.1";
// // 	filled_request._body = "";
// // 	filled_request.is_finished = true;
// // 	// filled_request._headers = ;
// // 	return (filled_request);
// // }

