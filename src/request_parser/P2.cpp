#include "../../Includes/request_parser/RequestParser.hpp"
#include "parser_internal.hpp"

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

bool Uri::operator==(const struct Uri &other) const {
	bool ret = true;
	
	if (!(this->full == other.full)) {
		std::cout << "Uri.full does not match!\n"; ret = false;
	}
	if (!(this->authority == other.authority)) {
		std::cout << "Uri.authority does not match!\n";
		ret = false;
	}
	if (!(this->host == other.host)) {
		std::cout << "Uri.host does not match!\n";
		ret = false;
	}
	if (!(this->port == other.port)) {
		std::cout << "Uri.port does not match!\n";
		ret = false;
	}
	if (!(this->path == other.path)) {
		std::cout << "Uri.path does not match!\n";
		ret = false;
	}
	if (!(this->query == other.query)) {
		std::cout << "Uri.query does not match!\n";
		ret = false;
	}
	if (!(this->form.is_origin_form == other.form.is_origin_form)) {
		std::cout << "Uri.form.is_origin_form does not match!\n";
		ret = false;
	}
	if (!(this->form.is_absolute_form == other.form.is_absolute_form)) {
		std::cout << "Uri.form.is_absolute_form does not match!\n";
		ret = false;
	}
	if (!(this->form.is_authority_form == other.form.is_authority_form)) {
		std::cout << "Uri.form.is_authority_form does not match!\n";
		ret = false;
	}
	if (!(this->form.is_asterisk_form == other.form.is_asterisk_form)) {
		std::cout << "Uri.form.is_asterisk_form does not match!\n";
		ret = false;
	}
	return (ret);
}

bool Request::operator==(const Request &other) const {
	bool ret = true;

	if (!(this->status_code == other.status_code)) {
		std::cout << "Request.status_code does not match!\n";
		ret = false;
	}
	if (this->status_code.has_value() && other.status_code.has_value()) {
		return (this->status_code == other.status_code);
	}
	if (!(this->method == other.method)) {
		std::cout << "Request.method does not match!\n";
		ret = false;
	}
	if (!(this->uri == other.uri)) {
		std::cout << "Request.uri does not match!\n";
		ret = false;
	}
	if (!(this->version == other.version)) {
		std::cout << "Request.version does not match!\n";
		ret = false;
	}
	if (!(this->headers == other.headers)) {
		std::cout << "Request.headers does not match!\n";
		ret = false;
	}
	if (!(this->body == other.body)) {
		std::cout << "Request.body does not match!\n";
		ret = false;
	}
	return (ret);
}

std::ostream& operator<<(std::ostream& output, Uri uri) {
	output << "\tURI type: ";
	if (uri.form.is_origin_form) {
		output << "origin-form ";
	}
	if (uri.form.is_absolute_form) {
		output << "aboslute-form ";
	}
	if (uri.form.is_authority_form) {
		output << "authority-form ";
	}
	if (uri.form.is_asterisk_form) {
		output << "asterisk-form ";
	}
	output << "\n";
	output << "\tURI.full: " << uri.full << "\n";
	output << "\tURI.authority: " << uri.authority << "\n";
	output << "\tURI.host: " << uri.host << "\n";
	output << "\tURI.port: " << uri.port << "\n";
	output << "\tURI.path: " << uri.path << "\n";
	output << "\tURI.query: " << uri.query << "";
	return (output);
};

std::ostream& operator<<(std::ostream &output, const Request &request) {
	output << "Method: " << request.method << "\n";
	output << "URI: \n" << request.uri << "\n";
	output << "Version: " << request.version << "\n";
	output << "Headers:\n";
	for (const auto &header : request.headers) {
		output << "\t" << header.first << " == " << header.second << "\n";
	}
	output << "Body: " << request.body << "\n";
	output << "Stutus code: " << request.status_code << "\n";
	return (output);
}

// only called via macro PARSE_ASSERT
//todo: add some hexdump of input since \r makes it hard to understand
bool RequestParser::parse_assertion_exec(bool cond, const char *str_cond, const char *file, const int line, const char *fn_str) {
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

	std::string cur_input = this->input;
	std::stringstream request_stream;
	request_stream << this->request;
	std::string cur_request = request_stream.str();
	std::string log_body;
	log_body += "RequestParser assertion in file " + std::string(file) + " line "
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

//todo: if messesge is not finished and exectly cut of where a space would be the request is falsly flaged is invalid
//example: "GET /abc"
const std::regex RequestParser::request_line_pat(REQUEST_LINE_PAT, std::regex::optimize);

const std::regex RequestParser::uri_pat(URI_PAT, std::regex::optimize);

// field name: either a valid or an unfinished name

const std::regex RequestParser::header_name_pat(FIELD_NAME, std::regex::optimize);
const std::regex RequestParser::header_value_pat(FIELD_VALUE, std::regex::optimize);

void RequestParser::setStatus(int status) {
	this->finished_request_line = true;
	this->finished_headers = true;
	this->finished = true;
	this->request.status_code = status;
}

RequestParser::RequestParser(std::string& input):
	input(input),
	pos(0)
{
}

RequestParser::~RequestParser(void){
}

bool RequestParser::parse_uri(void) {
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
		request.uri->form.is_origin_form = 1;
		request.uri->path = match[2].str();
		request.uri->query = match[3].str();
		PARSE_ASSERT(!match[4].matched);
		PARSE_ASSERT(!match[7].matched);
	} else if (match[4].matched) {
		request.uri->form.is_authority_form = 1;
		request.uri->authority = match[4].str();
		request.uri->host = match[5].str();
		request.uri->port = match[6].str();
		PARSE_ASSERT(!match[1].matched);
		PARSE_ASSERT(!match[7].matched);
	} else if (match[7].matched) {
		request.uri->form.is_absolute_form = 1;
		request.uri->authority = match[8].str();
		request.uri->host = match[9].str();
		request.uri->port = match[10].str();
		request.uri->path = match[12].str();
		request.uri->query = match[13].str();
		PARSE_ASSERT(!match[1].matched);
		PARSE_ASSERT(!match[4].matched);
	}
	return (true);
}

bool RequestParser::parse_request_line(void) {
	//std::cout << "parsing input of |" << this->input << "|" << std::endl;
	if (!this->input.size()) {
		return (false);
	}
	if (this->request.version.has_value()) {
		return (true);
	}
	std::smatch match;
	//std::cout << request_line_pat_str << std::endl;
	if (!std::regex_search(this->input.cbegin() + this->pos, this->input.cend(), match, this->request_line_pat)) {
		std::cout << "No match (invlaid request line?)\n";
		this->setStatus(400);
		return (true);
	}

	if (match[4].matched) {
		PARSE_ASSERT(!match[3].matched);
		std::cout << "Invalid method\n";
		this->setStatus(501);
		return (true);
	}
	if (!match[8].matched) {
		if (match[9].matched) {
			std::cout << "invalid request (not terminated with more content)\n";
			this->setStatus(400);
			return (true);
		}
		if (static_cast<long>(this->input.length()) - this->pos > REQUEST_LINE_MAX) {
			std::cout << "invalid request: request line too long\n";
			this->setStatus(400);
		} else {
			std::cout << "not terminated\n";
		}
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
	this->parse_uri();//todo: differentiate between too long request line and too long uri(414)
	if (!match[6].matched) {
		std::cout << "invalid version, 505\n";
		this->setStatus(505);
		return (true);
	}
	this->request.version = match.str(6);

	this->pos += match[2].length();
	return (true);
}

bool RequestParser::parse_headers(void) {
	if (this->request.status_code.has_value()) {
		return (true);
	}
	std::smatch	match;
	//std::cout << header_pat_str << std::endl;
	while (1) {
		if (this->input[static_cast<size_t>(this->pos) + 0] == '\r' && this->input[static_cast<size_t>(this->pos) + 1] == '\n') {
			this->finished_headers = true;
			this->pos += 2;
			//std::cout << "headers terminated\n";
			return (true);
		}
		if (!std::regex_search(this->input.cbegin() + this->pos, this->input.cend(), match, this->header_name_pat)) {
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
			if (std::isspace(key.back())) {
				std::cout << "Header name can not end with whitespace!\n";
				this->setStatus(400);
				return (true);
			}
			// todo: https://httpwg.org/specs/rfc9112.html#message.format sectio 6.1 end:
			// understand what to do if http version is 1.0 here
			std::transform(key.begin(), key.end(), key.begin(), [](char c) { return (std::tolower(c));});
			//todo: only advance if value is finished
			this->pos += match[1].length() + 1;
		} else {
			PARSE_ASSERT(match[2].matched);
			std::cout << "unfinished request header\n";
			return (false);
		}
		if (!std::regex_search(this->input.cbegin() + this->pos, this->input.cend(), match, this->header_value_pat)) {
			std::cout << "invalid header value\n";
			this->setStatus(400);
			return (true);
		}
		//for (size_t i = 0; i < match.size(); i++) {
		//	std::cout << "value[" << i << "]: |" << match[i].str() << "|\n";
		//}
		if (match[2].matched) {
			value = match.str(2);
			this->pos += match[1].length();
		} else {
			PARSE_ASSERT(match[3].matched);
			std::cout << "unfinished request header\n";
			return (false);
		}
		this->request.headers[key] = value;
	}
}

bool RequestParser::parse_not_encoded_body(size_t max_body_len) {
	size_t body_len;
	const std::string &body_len_str = this->request.headers["content-length"];
	const static std::regex body_len_pat(R"(^[ \t\v\f]*\d+[ \t\v\f]*$)", std::regex::optimize);
	if (!std::regex_match(body_len_str, body_len_pat)) {
		this->setStatus(400);
		std::cout << "invalid body len: " << body_len_str << std::endl;
		return (true);
	}
	try {
		body_len = std::stoul(body_len_str);
	} catch (const std::invalid_argument &e) {
		this->setStatus(400);
		std::cout << "invalid body len: " << body_len_str << std::endl;
		return (true);
	} catch (const std::out_of_range &e) {
		this->setStatus(413);
		std::cout << "invalid body len: " << body_len_str << std::endl;
		return (true);
	}
	std::cout << "not encoded, body len: " << body_len << std::endl;
	if (max_body_len < body_len) {
		std::cout << "invalid body len: " << body_len_str << "max body len: "
			<< max_body_len << std::endl;
		this->setStatus(413);
		return (true);
	}
	if (this->input.length() - static_cast<size_t>(this->pos) >= static_cast<size_t>(body_len)) {
		this->request.body = this->input.substr(static_cast<size_t>(this->pos), static_cast<size_t>(body_len));
		std::cout << "parsed un encoded body\n";
		return (true);
	}
	return (false);
}

// returns false if the body is not fullt received yet
bool RequestParser::parse_chunked(size_t max_body_len) {
	PARSE_ASSERT(this->pos > 0);
	if (this->input.size() <= static_cast<size_t>(this->pos)) {
		return (false);
	}
	while (1) {
		long old_pos = this->pos;
		auto crlf = input.find("\r\n", static_cast<size_t>(this->pos));
		if (crlf == std::string::npos) {
			this->pos = old_pos;
			return (false);
		}
		size_t chunk_ext_start;
		size_t chunk_size;
		try {
			chunk_size = std::stoul(this->input.substr(
				static_cast<size_t>(this->pos),
				crlf - static_cast<size_t>(this->pos)), &chunk_ext_start, 16);
		} catch (const std::out_of_range &e) {
			this->setStatus(413);
			return (true);
		} catch (const std::invalid_argument &e) {
			this->setStatus(400);
			return (true);
		}
		//todo: chunk-ext
		PARSE_ASSERT(crlf > static_cast<size_t>(this->pos));
		this->pos = 2 + static_cast<long>(crlf);
		if (chunk_size == 0) {
			break ;
		}
		if (this->request.body->length() + chunk_size > max_body_len) {
			this->setStatus(413);
			return (true);
		}
		if (this->input.length() < chunk_size + static_cast<size_t>(this->pos) + 2) {
			this->pos = old_pos;
			return (false);
		}
		if (this->input[static_cast<size_t>(this->pos) + chunk_size] != '\r'
			|| this->input[static_cast<size_t>(this->pos) + chunk_size + 1] != '\n')
		{
			this->setStatus(400);
			return (true);
		}
		if (this->request.body.has_value()) {
			this->request.body->append(this->input,
					static_cast<size_t>(this->pos), chunk_size);
		} else {
			this->request.body = this->input.substr(static_cast<size_t>(this->pos), chunk_size);
		}
		this->pos += chunk_size + 2;
	}
	size_t trailer_end = this->input.find("\r\n\r\n", static_cast<size_t>(this->pos));
	if (trailer_end == std::string::npos) {
		return (false);
	}
	//todo: trailers
	this->pos = static_cast<long>(trailer_end) + 4;
	return (true);
}

bool RequestParser::parse_encoded_body(size_t max_body_len) {
	std::string encoding_str = this->request.headers["transfer-encoding"];
	std::transform(encoding_str.begin(), encoding_str.end(), encoding_str.begin(), [](char c){return (std::tolower(c));});
	std::cout << "encoding: " << encoding_str << std::endl;
	if (this->request.headers.find("content-length") != this->request.headers.end()) {
		std::cout << "rejecting request since content-length and encoding was found\n";
		this->setStatus(400);
		return (true);
	}
	//todo: comma seperated splitting, not whitespace
	std::vector<std::string> encodings;
	std::stringstream ss;
	std::string encoding;
	ss << encoding_str;
	bool comma = true;
	while (ss >> encoding) {
		if (!comma) {
			this->setStatus(400);
			std::cout << "encodings were not comma seperated\n";
			return (true);
		}
		if (encoding.back() == ',') {
			encoding.pop_back();
			comma = true;
		} else {
			comma = false;
		}
		encodings.push_back(encoding);
	}
	// could add other encodings here
	while (encodings.size()) {
		if (encodings[0] == "chunked") {
			if (encodings.size() > 1) {
				this->setStatus(400);
				return (true);
			}
			if (!this->parse_chunked(max_body_len)) {
				return (false);
			}
			encodings.erase(encodings.begin());
		} else {
			std::cout << "Unsupported encoding: " << encodings[0] << "\n";
			this->setStatus(501);
			return (true);
		}
	}
	return (true);
}

// call this after verifying that there is a body by this rule:
//https://httpwg.org/specs/rfc9112.html#line.folding  section 6 (Message Body (introduction text))
//todo: check research ifrequests with a body and a method that does not have a body should be rejected
bool RequestParser::parse_body(size_t max_body_len) {
	if (this->request.body.has_value()) {
		return (true);
	}
	if (this->request.headers.find("transfer-encoding") != this->request.headers.end()) {
		return (parse_encoded_body(max_body_len));
	} else if (this->request.headers.find("content-length") != this->request.headers.end()) {
		return (this->parse_not_encoded_body(max_body_len));
	} else {
		std::cout << "no body\n";
		return (true);
	}
}

Request &&RequestParser::getRequest(void) {
	if (!this->request.body.has_value()) {
		this->request.body = "";
	}
	return (std::move(this->request));
}
/*
const char *dummy_input =
"POST http://example.com:443/test/path/file.txt HTTP/1.1\r\n" "Host: www.example.re\r\n"
"User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.0; en-US; rv:1.1)\r\n"
"Accept: text/html\r\n"
"Accept-Language: en-US, en; q=0.5\r\n"
"Accept-Encoding: gzip, deflate\r\n"
//"content-length:   10\r\n"
"Transfer-Encoding: chunked\r\n"
"\r\n"
"0x5\r\n"
"Hello\r\n"
"0x1\r\n"
" \r\n"
"0x6\r\n"
"World!\r\n"
"0x0\r\n"
"0xff\r\nnew message shouldn't be parsed\r\n"
;

int main(void) {
	std::string input(dummy_input);
	RequestParser p(input);
	Request re;

	if (p.parse_request_line()) {
		if (p.parse_headers()) {
			// in real webserv would select config here
			// 100 is a placeholder for value from the config
			if (p.parse_body(100)) {
				// save to get request here
				//re = p.getRequest();
			}
		}
	}
	//only for debugging: always get request
	re = p.getRequest();
	std::cout << re;
}
*/
