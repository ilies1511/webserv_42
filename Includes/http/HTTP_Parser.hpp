#ifndef HTTP_Parser_HPP
#define HTTP_Parser_HPP

#include <iostream>
#include <string>

class Request;
class Buffer;

class HTTP_Parser
{
	private:

	//OCF -- BEGIN
	public:
		HTTP_Parser(void);
		~HTTP_Parser(void);
	private:
		HTTP_Parser(const HTTP_Parser& other) = delete;
		HTTP_Parser& operator=(const HTTP_Parser& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN

		bool	parse(Buffer &InputBuffer, Request &request)
		{
			/* TODO:
				1. Suche nach der ersten Zeile und trenne sie ab
				2. Split Request Line (z.B. "GET /index.html HTTP/1.1")
				3. Read line by line headers, until Zeilenpaar (CRLF) found
				4. If Content-Length known --> check if full Body in Buffer
				5. Fill request Instance
					*this is jsust simplified Version
			*/
			return (true);
		}
	//Methodes -- END
};

#endif
