#include "Parser.hpp"

#include <string>
#include <unordered_map>


enum class Method {
	GET,
	POST,
	DELETE,
	INVALID,
};

enum class RequestHeader {
	
};

typedef struct Request {
	std::string	uri;
	Method		method;
	std::unordered_map<RequestHeader, std::string>
				headers;
	std::string	body;
	int			statusCode;

	Request(void):
		method(Method::INVALID),
		statusCode(200)
	{}

	~Request(void) {}

	Request(const Request& old):
		uri(old.uri),
		method(old.method),
		headers(old.headers),
		statusCode(old.statusCode)
	{}

	Request(Request&& old):
		uri(std::move(old.uri)),
		method(old.method),
		headers(std::move(old.headers)),
		statusCode(old.statusCode)
	{
		old.statusCode = 200;
		old.method = Method::INVALID;
	}

	Request operator=(const Request& old) {
		if (this == &old) {
			return (*this);
		}
		this->uri = old.uri;
		this->method = old.method;
		this->headers = old.headers;
		this->body = old.body;
		this->statusCode = old.statusCode;
		return (*this);
	}

	Request operator=(Request&& old) {
		if (this == &old) {
			return (*this);
		}
		this->uri = std::move(old.uri);
		this->method = old.method;
		old.method = Method::INVALID;
		this->headers = old.headers;
		this->body = old.body;
		this->statusCode = old.statusCode;
		old.statusCode = 200;
		return (*this);
	}

}	Request;


enum class TokenType {
	CRLF,
	LWS,
	TEXT,
};


typedef struct Token {
	TokenType	type;
} Token;

class Parser {
public:
	Parser(void) = delete;
	Parser(std::string &input);
	~Parser(void);

	void	parse(void);
	bool	finished;
	Request	getRequest(void);
private:
	Request		request;
	std::string	&input;
};

Parser::Parser(std::string &input)
	:input(input)
{
}




