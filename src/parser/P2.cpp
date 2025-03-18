#include <istream>
#include <string>
#include <optional>
#include <unordered_map>
#include <regex>
#include <iostream>
#include <cassert>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <ctime>


/* for later TODO:
 * make code efficient
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
	std::string	port;
	std::string	path;
	std::string	query;
} uri;

typedef struct Request {
	std::optional<std::string>						method = std::nullopt;
	std::optional<Uri>								uri = std::nullopt;
	std::optional<std::string>						version = std::nullopt;
	std::unordered_map<std::string, std::string>	headers;
	std::string										body;
	std::optional<int>								status_code = std::nullopt;
} Request;


std::ostream& operator<<(std::ostream& output, Uri uri) {
	output << "\tURI.full: " << uri.full << "\n";
	output << "\tURI.authority: " << uri.authority << "\n";
	output << "\tURI.host: " << uri.host << "\n";
	output << "\tURI.port: " << uri.port << "\n";
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
		output << "\t" << header.first << " == " << header.second << "\n";
	}
	output << "Body: " << request.body << "\n";
	output << "Stutus code: " << request.status_code << "\n";

	return (output);
}

class Parser {
public:
				Parser(void) = delete;
				Parser(std::string &input);
				~Parser();
	bool		parse_request_line(void);
	bool		parse_uri(void);
	bool		parse_headers(void);
	Request		&&getRequest(void);

	Request		request;
	std::string &input;
private:

	void	setStatus(int status);
	bool	finished_request_line = false;
	bool	finished_headers = false;
	bool	finished = false;

	size_t		pos;
	const static std::regex	request_line_pat;
	const static std::regex	uri_pat;
	const static std::regex	header_name_pat;
	const static std::regex	header_value_pat;
};

//todo: add some hexdump of input since \r makes it hard to understand
bool parse_assertion_exec(bool cond, const Parser &parser, const char *str_cond, const char *file, const int line, const char *fn_str) {
	if (cond) {
		return (cond);
	}
	const char *log_file_name = "parser_asserts.log";

	struct stat stats;
	stat(log_file_name, &stats);
	std::ifstream in_log_stream(log_file_name);
	std::string old_log_file;
	old_log_file.reserve(static_cast<size_t>(stats.st_size));
	in_log_stream.read(old_log_file.data(), static_cast<long>(stats.st_size));
	in_log_stream.close();

	std::string time_stamp;
	time_stamp.resize(30);
	std::time_t now = std::time(nullptr);
	std::tm tm_buf;
	localtime_r(&now, &tm_buf);
	size_t time_len = std::strftime(time_stamp.data(), time_stamp.size(), "%d-%m %H:%M", &tm_buf);
	time_stamp.resize(time_len);
	time_stamp += "\n";

	std::string cur_input = parser.input;
	std::stringstream request_stream;
	request_stream << parser.request;
	std::string cur_request = request_stream.str();
	std::string log_body;
	log_body += "Parser assertion in file " + std::string(file) + " line "
		+ std::to_string(line) + "(function '" + fn_str + "')!\n";
	log_body += std::string("Assert condion: '(") + std::string(str_cond) + ")'\n";
	log_body += "------------";
	log_body += "Current input:\n";
	log_body += cur_input;
	log_body += "------------";
	log_body += "Current request:\n";
	log_body += cur_request;

	std::string header = "*****************************************************************\n";
	std::string footer = "-----------------------------------------------------------------\n";

	std::string log_msg = header;
	log_msg += time_stamp;
	log_msg += log_body;
	log_msg += footer;
	std::ofstream out_log_stream(log_file_name, std::ios::app);
	std::cerr << log_msg;

	out_log_stream.write(log_msg.data(), static_cast<long>(log_msg.length()));
	out_log_stream.close();
	exit(1);
	return (cond);
}

// only use within parser class
#define PARSE_ASSERT(cond) parse_assertion_exec(cond, *this, #cond, __FILE__, __LINE__, __FUNCTION__)


//const std::regex Parser::request_line_pat(R"((^GET|^POST|^DELETE) ([.]+)(\d+\.\d+)\r$)");

const char *request_line_pat_str = "(^((?:(GET|POST|DELETE)|([a-zA-Z]+)) (\\S+)? (?:(HTTP\\/1\\.1)|(\\w+\\/\\d+\\.\\d+))(\r\n)?)([\\s\\S]*)?)";
const std::regex Parser::request_line_pat(request_line_pat_str);

#define PATH "(\\/(?:[^S\\/\\?\\#]+\\/?)+)"//todo: currently the path can have anything that is not space, '?' or '#'
#define QUERY "(?:(?:\\?)([^\\s\\#]*))"
#define URI_TERM "(?:(?:\\#\\S*)|$)" // either end of uri or # folloed by some non whitespace

#define ORIGIN_FORM "(" PATH QUERY "?" URI_TERM ")"

#define HOST "([\\w\\.]+)"
#define PORT "([1-9]\\d{0,3})"

#define AUTHORITY_FORM "(" HOST ":" PORT ")"

#define ABSOLUTE_FORM "(http\\:\\/\\/" AUTHORITY_FORM ORIGIN_FORM ")"

//ASTERISK_FORM : not implemented

const char *uri_pat_str = "(?:" ORIGIN_FORM "|" AUTHORITY_FORM "|" ABSOLUTE_FORM ")";
const std::regex Parser::uri_pat(uri_pat_str);

// field name: either a valid or an unfinished name
#define FIELD_NAME "(?:(?:([^\\:\n]+)\\:(?:[\\s\\S]*))|([^\\:\n]*$))"
#define FIELD_VALUE "(?:(?:([ \t\f\v]*([^\r\n]+)\r\n)[\\s\\S]*)|([ \t\f\v]*[^\r\n]*\r?$))"
const std::regex Parser::header_name_pat(FIELD_NAME);
const std::regex Parser::header_value_pat(FIELD_VALUE);

void Parser::setStatus(int status) {
	this->finished_request_line = true;
	this->finished_headers = true;
	this->finished = true;
	this->request.status_code = status;
}

Parser::Parser(std::string& input):
	input(input),
	pos(0)
{
}

Parser::~Parser(void){
}

bool Parser::parse_uri(void) {
	//std::cout << uri_pat_str << std::endl;
	std::smatch match;
	if (!std::regex_match(this->request.uri->full, match, this->uri_pat)) {
		std::cout << "invalid request uri\n";
		this->setStatus(400); //todo: is 400 correct?
		return (true);
	}
	//for (size_t i = 0; i < match.size(); i++) {
	//	std::cout << "match[" << i << "]: |" << match[i].str() << std::endl;
	//}
	if (match[1].matched) {
		// origin-form
		PARSE_ASSERT(!match[4].matched);
		PARSE_ASSERT(!match[7].matched);
		request.uri->path = match[2].str();
		request.uri->query = match[3].str();
	} else if (match[4].matched) {
		// authority-form
		PARSE_ASSERT(!match[1].matched);
		PARSE_ASSERT(!match[7].matched);
		request.uri->authority = match[4].str();
		request.uri->host = match[5].str();
		request.uri->port = match[6].str();
	} else if (match[7].matched) {
		//absolute_form
		PARSE_ASSERT(!match[1].matched);
		PARSE_ASSERT(!match[4].matched);
		request.uri->authority = match[8].str();
		request.uri->host = match[9].str();
		request.uri->port = match[10].str();
		request.uri->path = match[12].str();
		request.uri->query = match[13].str();
	}
	return (true);
}

bool Parser::parse_request_line(void) {
	std::smatch match;
	//std::cout << request_line_pat_str << std::endl;
	if (!std::regex_match(this->input, match, this->request_line_pat)) {
		std::cout << "No match (invlaid request line?)\n";
		this->setStatus(400);
		return (true);
	}
	//for (size_t i = 0; i < match.size(); i++) {
	//	std::cout << "match[" << i << "]: |" << match[i].str() << std::endl;
	//}
	if (match[4].matched) {
		PARSE_ASSERT(!match[3].matched);
		std::cout << "Invalid method\n";
		this->setStatus(501);
		return (true);
	}
	if (!match[8].matched) {
		if (match[9].matched) {
			std::cout << "invalid request (not terminated with more content)\n";
			return (true);
		}
		std::cout << "not terminated\n";
		return (false);
	}
	this->request.method = match[3].str();
	Uri uri;
	if (!match[3].matched) {
		std::cout << "invalid uri\n";
		this->setStatus(400);
		return (true);
	}
	uri.full = match.str(5);
	this->request.uri = std::move(uri);
	this->parse_uri();
	if (!match[6].matched) {
		std::cout << "invalid version, 505\n";
		this->setStatus(505);
		return (true);
	}
	this->request.version = match.str(6);

	//todo: needs more efficient solution later
	this->input.erase(0, static_cast<size_t>(match[2].length()));

	return (true);
}

bool Parser::parse_headers(void) {
	std::smatch	match;
	//std::cout << header_pat_str << std::endl;
	while (1) {
		if (this->input[0] == '\r' && this->input[1] == '\n') {
			this->finished_headers = true;
			this->input.erase(0, 2);
			std::cout << "headers terminated\n";
			return (true);
		}
		if (!std::regex_match(this->input, match, this->header_name_pat)) {
			std::cout << "invalid header name\n";
			this->setStatus(400);
			return (true);
		}

		//for (size_t i = 0; i < match.size(); i++) {
		//	std::cout << "key[" << i << "]: |" << match[i].str() << "|\n";
		//}
		std::string key;
		std::string value;
		if (match[1].matched) {
			key = match.str(1);
			//todo: only advance if value is finished
			this->input.erase(0, match[1].length() + 1);
		} else {
			PARSE_ASSERT(match[2].matched);
			std::cout << "unfinished request header\n";
			return (false);
		}
		if (!std::regex_match(this->input, match, this->header_value_pat)) {
			std::cout << "invalid header value\n";
			this->setStatus(400);
			return (true);
		}
		//for (size_t i = 0; i < match.size(); i++) {
		//	std::cout << "value[" << i << "]: |" << match[i].str() << "|\n";
		//}
		if (match[2].matched) {
			value = match.str(2);
			this->input.erase(0, match[1].length());
		} else {
			PARSE_ASSERT(match[3].matched);
			std::cout << "unfinished request header\n";
			return (false);
		}
		this->request.headers[key] = value;
	}
}

Request &&Parser::getRequest(void) {
	return (std::move(this->request));
}

const char *dummy_input =
"GET example.com:443 HTTP/1.1\r\n"
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
	Request re;
	if (p.parse_request_line()) {
		if (p.parse_headers()) {
			// save to get request here
			//re = p.getRequest();
		}
	}
	//only for debugging: always get request
	re = p.getRequest();
	std::cout << re;
}
