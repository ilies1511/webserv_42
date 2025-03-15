#include "Parser.hpp"


#include <string>
#include <unordered_map>
#include <iostream>
#include <cassert>


#define FIRST_LINE_MAX
#define HEADER_NAME_MAX
#define HEADER_VAL_MAX

typedef struct Request {
	std::string	uri;
	std::string	method;
	std::unordered_map<std::string, std::string>
				headers;
	std::string	body;
	int			statusCode;

	Request(void):
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
		method(std::move(old.method)),
		headers(std::move(old.headers)),
		statusCode(old.statusCode)
	{
		old.statusCode = 200;
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
		this->method = std::move(old.method);
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
	std::string	content;
} Token;

enum class ParserState {
	START_LINE,
	HEADERS,
	BODY,
};

class Parser {
public:
	Parser(void) = delete;
	Parser(std::string &input);
	Parser(std::string &input, int maxBodySize);
	~Parser(void);

	bool	parse(void);
	Request	getRequest(void);
private:
	void		parseStartLine(void);
	void		parseHeader(void);
	void		parseBody(void);
	std::string	&input;
	const int	maxBodySize;
	Request		request;
	ParserState	state;
	bool		waitInput;
};

Parser::Parser(std::string &input, int maxBodySize):
	input(input),
	maxBodySize(maxBodySize),
	state(ParserState::START_LINE)
{}

Parser::Parser(std::string &input):
	Parser(input, -1)
{}

Parser::~Parser(void) {
}

void Parser::parseStartLine(void) {
	std::string line; //1. = getline();
	if (this->waitInput) {
		return ;
	}
	//2. refc2616 section 4.1: if empty line SHOULD skip it
	//3. check that exactly 3 words are in the line
	// else set status for invalid request and return
	std::string method;
	std::string uri;
	std::string version;
}

void Parser::parseHeader(void) {
}

void Parser::parseBody(void) {
}

//returns true if the full request was received
bool Parser::parse(void) {
	this->waitInput = false;
	while (!this->waitInput && this->request.statusCode == 200) {
		switch (this->state) {
			case (ParserState::START_LINE): {
				this->parseStartLine();
				break ;
			}
			case (ParserState::HEADERS): {
				this->parseHeader();
				break ;
			}
			case (ParserState::BODY): {
				this->parseBody();
				if (!this->waitInput) {
					return (true);
				}
				break ;
			}
			default: assert(0);
		}
	}
	if (this->request.statusCode == 200) {
		return (false);
	}
	// request status code was set: invalid request
	return (true);
}

Request Parser::getRequest(void) {
	return (this->request);
}

