// #ifndef HTTP_Parser_HPP
// #define HTTP_Parser_HPP

// #include <iostream>
// #include <string>
// // #include "Request.hpp"
// #include "RequestParser.hpp"


// // class Request;
// /*
// 	HTTP_Parser used, just as an sort of Interface, holding methodes in order
// 	to send filled Request Instance
// */
// class Buffer;

// class Parser
// {
// 	public:
// 		bool	_parsing_finished = false;
// 	private:

// 	//OCF -- BEGIN
// 	public:
// 	Parser(void);
// 	~Parser(void);
// 	private:
// 	Parser(const Parser& other) = delete;
// 	Parser& operator=(const Parser& other) = delete;
// 	//OCF -- END

// 	//Methodes -- BEGIN

// 	public:
// 		//parse() ist der EntryPoint fuer die Parser Logik
// 		// bool	parse(Buffer &InputBuffer, Request &request)
// 		// {
// 		// 	(void)InputBuffer;
// 		// 	/* TODO:
// 		// 		1. Suche nach der ersten Zeile und trenne sie ab
// 		// 		2. Split Request Line (z.B. "GET /index.html HTTP/1.1")
// 		// 		3. Read line by line headers, until Zeilenpaar (CRLF) found
// 		// 		4. If Content-Length known --> check if full Body in Buffer
// 		// 		5. Fill request Instance
// 		// 			*this is jsust simplified Version
// 		// 	*/
// 		// 	//Hardgecodet;
// 		// 	request._method = "GET";
// 		// 	request._uri = "/";
// 		// 	return (true);
// 		// }
// 		Request entry_parse(Buffer &InputBuffer, Request &filled_request);
// 		// Request entry_parse(Buffer &InputBuffer);
// 		bool	parse_header(Buffer &InputBuffer, Request &request);
// 		bool	parse_body(Buffer &InputBuffer, Request &request);
// 		bool	request_line(Buffer &InputBuffer, Request &request);
// 	//Methodes -- END
// };

// #endif
