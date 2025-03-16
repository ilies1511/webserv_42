#include <istream>
#include <string>
#include <optional>
#include <unordered_map>
#include <regex>
#include <iostream>
#include <cassert>

/* for later TODO:
https://httpwg.org/specs/rfc9112.html#message.format
1.: "A sender MUST NOT generate a bare CR (a CR character not immediately followed by LF) within any protocol elements other than the content. A recipient of such a bare CR MUST consider that element to be invalid or replace each bare CR with SP before processing the element or forwarding the message"
2.: "In the interest of robustness, a server that is expecting to receive and parse a request-line SHOULD ignore at least one empty line (CRLF) received prior to the request-lin"
3.: "A recipient that receives whitespace between the start-line and the first header field MUST either reject the message as invalid or consume each whitespace-preceded line without further processing of it (i.e., ignore the entire line, along with any subsequent lines preceded by whitespace, until a properly formed header field is received or the header section is terminated"
4.: "When a server listening only for HTTP request messages, or processing what appears from the start-line to be an HTTP request message, receives a sequence of octets that does not match the HTTP-message grammar aside from the robustness exceptions listed above, the server SHOULD respond with a 400 (Bad Request) response and close the connection"
5.: ""
6.: ""
7.: ""
8.: ""
9.: ""
*/
/*
 *** Request-line:
 - SP is the recommend seperator, tho any whitespace besided CRLF might be interpreted as such
 - leading and trailing whitedpace MAY be skipped, tho it is not recommend
 **1.method:
 - method token followed by a single SP
 - case sensitive
 - 501: not recognized or not implemented (should)
 - 405: not allowed (should)
 **2.tagget
 -"  request-target = origin-form
                 / absolute-form
                 / authority-form
                 / asterisk-form -- asterisk-form: won't implement
	"
 - no whitespace allowed
 - SHOULD: on invalid target either 400(Bad Request) or 301(Moved Permanently) to redir to the valid one
 - URI in our case) followed by a single SP
 - SHOULD NOT auto correct (without 301) of invalid targets
 **3.Version:
 -case sensitive, "HTTP/1.1
*** Headers:
**Host:
- a client MUST send a Host header-field
section 3.2: "A client MUST send a Host header field (Section 7.2 of [HTTP]) in all HTTP/1.1 request messages. If the target URI includes an authority component, then a client MUST send a field value for Host that is identical to that authority component, excluding any userinfo subcomponent and its "@" delimiter (Section 4.2 of [HTTP]). If the authority component is missing or undefined for the target URI, then a client MUST send a Host header field with an empty field value.
A server MUST respond with a 400 (Bad Request) status code to any HTTP/1.1 request message that lacks a Host header field and to any request message that contains more than one Host header field line or a Host header field with an invalid field value."

*/
#ifndef REQUEST_LINE_MAX
//https://httpwg.org/specs/rfc9112.html#message.format '3. Request Line':
//'It is RECOMMENDED that all HTTP senders and recipients support, at a
//minimum, request-line lengths of 8000 octets'
# define REQUEST_LINE_MAX 8000 //if it's longer than this give a 501
#endif // REQUEST_LINE_MAX

#ifndef URI_MAX
# define URI_MAX //give a 414
#endif // URI_MAX

typedef struct Uri {
	std::string	full;
	std::string	authority;
	std::string	host;
	std::string	path;
	std::string	query;
} uri;

typedef struct Request {
	std::optional<std::string>						method = std::nullopt;
	std::optional<Uri>								uri = std::nullopt;
	std::optional<std::string>						version = std::nullopt;
	std::unordered_map<std::string, std::string>	headers;
	bool											header_term = false;
	std::string										body;
	std::optional<int>								status_code = std::nullopt;
	bool											finished = false;
} Request;

std::ostream& operator<<(std::ostream& output, Uri uri) {
	output << "\tURI.full: " << uri.full << "\n";
	output << "\tURI.authority: " << uri.authority << "\n";
	output << "\tURI.host: " << uri.host << "\n";
	output << "\tURI.path: " << uri.path << "\n";
	output << "\tURI.query: " << uri.query << "";
	return (output);
};

template<class T>
static std::ostream& operator<<(std::ostream& output, std::optional<T> val) {
	if (val.has_value()) {
		output << *val;
	} else {
		output << "No value";
	}
	return (output);
}

std::ostream& operator<<(std::ostream &output, const Request &request) {
	output << "Method: " << request.method << "\n";
	output << "URI: " << request.uri << "\n";
	output << "Version: " << request.version << "\n";
	output << "Headers:\n";
	for (const auto &header : request.headers) {
		output << "\t" << header.first << ": " << header.second << "/n";
	}
	output << "Header termination: " << request.header_term << "\n";
	output << "Body: " << request.body << "\n";
	output << "Stutus code: " << request.status_code << "\n";
	output << "Finished: " << request.finished << "\n";
	return (output);
}

class Parser {
public:
				Parser(void) = delete;
				Parser(std::string &input);
				~Parser();
	Request		parse(void);
	void		parse_request_line(void);
	void		parse_uri(void);
private:
	Request		request;
	std::string &input;
	size_t		pos;
	const static std::regex	request_line_pat;
	const static std::regex	uri_pat;
	const static std::regex	query_pat;

	const static std::regex line_term_pat;
	const static std::regex header_pat;
};

//const std::regex Parser::request_line_pat(R"((^GET|^POST|^DELETE) ([.]+)(\d+\.\d+)\r$)");

const std::regex Parser::request_line_pat(R"((^GET|^POST|^DELETE)? (\S+)? (HTTP/1\.1)?(\r\n)?([\s\S]+)?)");
#define PATH "(\\/(?:[^S\\/\\?\\#]+\\/?)+)"
#define QUERY "(?:(?:(?:\\?)([^\\s\\#]*))"
#define URI_TERM "(?:(?:\\#\\S*)|$)"
const char *uri_pat_str = "(?:" PATH QUERY "?" URI_TERM "))";
const std::regex Parser::uri_pat(uri_pat_str);//todo: currently the path can have anything that is not space, '?' or '#'


Parser::Parser(std::string& input):
	input(input),
	pos(0)
{
}

Parser::~Parser(void){
}

void Parser::parse_uri(void) {
	std::cout << uri_pat_str << std::endl;
	std::smatch match;
	if (std::regex_match(this->request.uri->full, match, this->uri_pat)) {
		std::cout << "Origin-form\n";
		this->request.uri->path = match[1].str();
		this->request.uri->query = match[2].str();
		for (size_t i = 0; i < match.size(); i++) {
			std::cout << "match[" << i << "]: |" << match[i] << "|\n";
		}
		//if (std::regex_match(this->request.uri->full.begin() + match[0].str().length(), this->request.uri->full.end(), match, this->query_pat)) {
		//	this->request.uri->query = match[1].str();
		//} else {
		//	std::cout << "No query\n";
		//}
		return ;
	}
	std::cout << "not origin-form\n";
}

void Parser::parse_request_line(void) {
	std::smatch match;
	if (std::regex_match(this->input, match, this->request_line_pat)) {
		if (!match[4].matched) {
			if (match[5].matched) {
				std::cout << "invalid request (not terminated with more content)\n";
				for (size_t i = 1; i < match.size(); i++) {
					std::cout << "match[" << i << "] = |" << match[i].str() << "|\n";
				}
				return ;
			}
			std::cout << "not terminated\n";
			return ;
		}
		if (!match[1].matched) {
			std::cout << "Invalid method\n";
			return ;
		}
		this->request.method = match.str(1);
		Uri uri;
		if (!match[2].matched) {
			//invalid uri, 400 or 301
			std::cout << "invalid uri\n";
		}
		uri.full = match.str(2);
		this->request.uri = std::move(uri);
		this->parse_uri();
		if (!match[3].matched) {
			std::cout << "invalid version, 505\n";
			return ;
		}

		this->request.version = match.str(3);

	} else {
		std::cout << "No match (invlaid request?)\n";
	}
}

Request Parser::parse(void) {

	this->parse_request_line();

	return (this->request);
}

const char *dummy_input =
"DELETE /index.html/?abc#sdf HTTP/1.1\r\n"
"Host: www.example.re\r\n"
"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.1)\r\n"
"Accept: text/html\r\n"
"Accept-Language: en-US, en; q=0.5\r\n"
"Accept-Encoding: gzip, deflate\r\n"
"\r\n"
;

int main(void) {
	std::string input(dummy_input);
	Parser p(input);
	Request re = p.parse();
	
	std::cout << re;
}
