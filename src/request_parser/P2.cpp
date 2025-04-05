#include "../../Includes/request_parser/RequestParser.hpp"
#include "parser_internal.hpp"

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
}

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
	//std::ifstream in_log_stream(log_file_name);
	//std::string old_log_file;
	//old_log_file.reserve(static_cast<size_t>(stats.st_size));
	//in_log_stream.read(old_log_file.data(), static_cast<long>(stats.st_size));
	//in_log_stream.close();

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
	this->setStatus(500);
	return (cond);
}

const std::regex RequestParser::method_pat(METHOD_PAT, std::regex::optimize);

const std::regex RequestParser::uri_pat(URI_PAT, std::regex::optimize);


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

static bool is_hex_digit(char c) {
	if ((c >= '0' && c <= '9')
		|| (c >= 'A' && c <= 'F')
		|| (c >= 'a' && c <= 'f')
	) {
		return (true);
	}
	return (false);
}

char RequestParser::to_hex(char c) {
	if (c >= '0' && c <= '9') {
		return (static_cast<char>(static_cast<int>(c) - static_cast<int>('0')));
	} else if (c >= 'A' && c <= 'F') {
		return (static_cast<char>(static_cast<int>(c) - static_cast<int>('A') + 10));
		// return (c - 'A' + 10);
	} else if (c >= 'a' && c <= 'f') {
		return (static_cast<char>(static_cast<int>(c) - static_cast<int>('a') + 10));
		// return (c - 'a' + 10);
	} else {
		return (PARSE_ASSERT(0));
	}
}

std::string RequestParser::uri_decode(const std::string &str) {
	std::string	ret;
	ret.reserve(str.length());
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] != '%') {
			ret += str[i];
		} else {
			i++;
			if (i + 1 >= str.size() || !is_hex_digit(str[i]) || !is_hex_digit(str[i + 1])) {
				std::cout << "invalid uri encoding: |" << str.substr(i + 1, 2) << "|\n";
				this->request.status_code = 400;
				return (ret);
			}
			ret += static_cast<char>(static_cast<int>(this->to_hex(str[i])) * 16 + static_cast<int>(this->to_hex(str[i + 1])));
			i++;
		}
	}
	return (ret);
}

bool RequestParser::parse_uri(void) {
	if (this->request.uri.has_value() || this->request.status_code.has_value()) {
		return (true);
	}
	while (this->pos < this->input.size() && this->input[this->pos] == ' ') {
		this->pos++;
	}
	size_t uri_term = this->input.find(' ', this->pos);
	if ((uri_term - this->pos) > URI_MAX) {
		this->setStatus(414);
		std::cout << "Uri too long!\n";
		return (true);
	}
	if (uri_term == std::string::npos) {
		std::cout << "Not terminated uri\n";
		return (false);
	}
	Uri uri;
	uri.full = this->input.substr(this->pos, uri_term - this->pos);
	this->request.uri = std::move(uri);

	std::smatch match;
	if (!std::regex_match(this->request.uri->full, match, this->uri_pat)) {
		std::cout << "invalid request uri: " << this->request.uri->full << "\n";
		this->setStatus(400); //todo: is 400 correct?
		return (true);
	}
	//for (size_t i = 0; i < match.size(); i++) {
	//	std::cout << "match[" << i << "]: |" << match[i].str() << "|\n";
	//}
	if (match[16].matched) {
		// asterisk-form
		this->request.status_code = 501;
		std::cout << "Asterisk form uri: OPTIONS method not implemented\n";
		this->request.uri->path = "*";
		return (true);
	}
	if (match[1].matched) {
		// origin-form
		request.uri->form.is_origin_form = 1;
		request.uri->path = match[2].str();
		request.uri->query = match[3].str();
		PARSE_ASSERT(!match[4].matched);
		PARSE_ASSERT(!match[8].matched);
	} else if (match[4].matched) {
		// authority-form
		request.uri->form.is_authority_form = 1;
		request.uri->authority = match[4].str();
		if (match[6].matched) {
			//IPv6
			request.uri->host = match[6].str();
		} else {
			request.uri->host = match[5].str();
		}
		request.uri->port = match[7].str();
		PARSE_ASSERT(!match[1].matched);
		PARSE_ASSERT(!match[8].matched);
	} else if (match[8].matched) {
		// absolute-form
		request.uri->form.is_absolute_form = 1;
		request.uri->authority = match[9].str();
		if (match[11].matched) {
			//IPv6
			request.uri->host = match[11].str();
		} else {
			request.uri->host = match[10].str();
		}
		request.uri->port = match[12].str();
		request.uri->path = match[14].str();
		request.uri->query = match[15].str();
		PARSE_ASSERT(!match[1].matched);
		PARSE_ASSERT(!match[4].matched);
	}
	this->request.uri->path = this->uri_decode(this->request.uri->path);
	this->request.uri->query = this->uri_decode(this->request.uri->query);

	this->pos = uri_term;

	return (true);
}

//returns true if the method got parsed or the request is finished
//sets status on parse error
bool RequestParser::parse_method(void) {
	std::smatch match;
	if (this->request.method.has_value() || this->request.status_code.has_value()) {
		return (true);
	}
	if (!std::regex_search(this->input.cbegin() + static_cast<long>(this->pos), this->input.cend(), match, this->method_pat)) {
		std::cout << "No match (invlaid request line (no method))\n";
		this->setStatus(400);
		return (true);
	}
	if (match[1].matched) {
		PARSE_ASSERT(!match[2].matched && !match[3].matched);
		this->request.method = match.str(1);
		this->pos += static_cast<size_t>(match[0].length());
		return (true);
	} else if (match[2].matched) {
		PARSE_ASSERT(!match[1].matched && !match[3].matched);
		std::cout << "Invalid method\n";
		this->setStatus(501);
		return (true);
	} else if (match[3].matched) {
		if (this->input.length() - this->pos > REQUEST_LINE_MAX) { //todo: could add method too long here
			std::cout << "invalid request: request line too long\n";
			this->setStatus(400);
			return (true);
		}
		std::cout << "not terminated method\n";
		return (false);
	} else {
		PARSE_ASSERT(0);
		return (true);
	}
}

bool RequestParser::parse_version(void) {
	while (this->pos < this->input.size() && this->input[this->pos] == ' ') {
		this->pos++;
	}
	if (this->request.status_code.has_value()) {
		return (true);
	}
	if (this->request.version.has_value()) {
		return (true);
	}
	size_t len = std::strlen("HTTP/1.1\r\n");
	std::string sub = this->input.substr(this->pos, len);
	if (sub == "HTTP/1.1\r\n") {
		this->request.version = "HTTP/1.1";
		this->pos += static_cast<size_t>(len);
		return (true);
	}
	const std::string ver = "HTTP/1.1\r\n";
	for (size_t i = 0; i < sub.size(); i++) {
		if (sub[i] != ver[i]) {
			if (sub.find(' ') != std::string::npos) {
				this->setStatus(400);
				std::cout << "Invalid space in request line\n";
				return (true);
			}
			std::cout << " Invlaid version: " << sub << "\n";
			this->setStatus(505);
			return (true);
		}
	}
	std::cout << "Unfinished version\n";
	return (false);
}

bool RequestParser::parse_request_line(void) {
	if (this->request.version.has_value() || this->request.status_code.has_value()) {
		return (true);
	}
	if (!this->parse_method()) {
		return (false);
	}
	if (this->request.status_code.has_value()) {
		return (true);
	}
	if (!this->parse_uri()) {
		return (false);
	}
	if (this->request.status_code.has_value()) {
		return (true);
	}
	return (this->parse_version());
}

std::string RequestParser::parse_header_name(void) {
	std::smatch match;
	std::string name;

	if (!std::regex_search(this->input.cbegin() + static_cast<long>(this->pos), this->input.cend(), match, this->header_name_pat)) {
		std::cout << "invalid header name\n";
		this->setStatus(400);
		return ("");
	}

	//for (size_t i = 0; i < match.size(); i++) {
	//	std::cout << "key[" << i << "]: |" << match[i].str() << "|\n";
	//}
	if (match[1].matched) {
		name = match.str(1);
		if (std::isspace(name.back())) {
			std::cout << "Header name can not end with whitespace!\n";
			this->setStatus(400);
			return ("");
		}
		std::transform(name.begin(), name.end(), name.begin(), [](char c) { return (std::tolower(c));});
		this->pos += static_cast<size_t>(match[1].length()) + 1;
		return (name);
	} else {
		PARSE_ASSERT(match[2].matched);
		std::cout << "unfinished request header\n";
		this->request.status_code = 1000; // indicate unfinished
		return ("");
	}
}

std::string RequestParser::parse_header_value(void) {
	std::smatch match;
	std::string value;

	if (!std::regex_search(
		this->input.cbegin() + static_cast<long>(this->pos),
		this->input.cend(), match, this->header_value_pat))
	{
		std::cout << "invalid header value\n";
		this->setStatus(400);
		return ("");
	}
	//for (size_t i = 0; i < match.size(); i++) {
	//	std::cout << "value[" << i << "]: |" << match[i].str() << "|\n";
	//}
	if (match[2].matched) {
		value = match.str(2);
		this->pos += static_cast<size_t>(match[1].length());
		return (value);
	} else {
		PARSE_ASSERT(match[3].matched);
		std::cout << "unfinished request header\n";
		this->request.status_code = 1000;
		return ("");
	}
}

bool RequestParser::parse_headers(void) {
	if (this->request.status_code.has_value() || this->finished_headers) {
		return (true);
	}
	std::smatch	match;
	while (1) {
		if (this->input[this->pos + 0] == '\r' && this->input[this->pos + 1] == '\n') {
			this->finished_headers = true;
			this->pos += 2;
			return (true);
		}
		size_t old_pos = this->pos;
		std::string key = this->parse_header_name();
		if (this->request.status_code.has_value()) {
			if (this->request.status_code == 1000) {// indicates unfinished
				this->request.status_code = std::nullopt;
				this->pos = old_pos;
				return (false);
			}
			return (true);
		}
		std::string value = this->parse_header_value();
		if (this->request.status_code.has_value()) {
			if (this->request.status_code == 1000) {// indicates unfinished
				this->request.status_code = std::nullopt;
				this->pos = old_pos;
				return (false);
			}
			return (true);
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
		std::cout << "invalid body len: " << body_len_str << "\n";
		return (true);
	}
	try {
		body_len = std::stoul(body_len_str);
	} catch (const std::invalid_argument &e) {
		this->setStatus(400);
		std::cout << "invalid body len: " << body_len_str << "\n";
		return (true);
	} catch (const std::out_of_range &e) {
		this->setStatus(413);
		std::cout << "invalid body len: " << body_len_str << "\n";
		return (true);
	}
	std::cout << "not encoded, body len: " << body_len << "\n";
	if (max_body_len < body_len) {
		std::cout << "invalid body len: " << body_len_str << "max body len: "
			<< max_body_len << "\n";
		this->setStatus(413);
		return (true);
	}
	if (this->input.length() - this->pos >= body_len) {
		this->request.body = this->input.substr(this->pos, body_len);
		std::cout << "parsed un encoded body\n";
		return (true);
	}
	return (false);
}

// returns false if the body is not fullt received yet
bool RequestParser::parse_chunked(size_t max_body_len) {
	PARSE_ASSERT(this->pos > 0);
	if (this->input.size() <= this->pos) {
		return (false);
	}
	std::cout << "Parsing chunked\n";
	while (1) {
		size_t old_pos = this->pos;
		if (!is_hex_digit(this->input[this->pos])) {
			std::cout << "invalid chunk start\n";
			this->setStatus(400);
			return (true);
		}
		auto crlf = input.find("\r\n", this->pos);
		if (crlf == std::string::npos) {
			this->pos = old_pos;
			return (false);
		}
		size_t chunk_ext_start;
		size_t chunk_size;
		try {
			chunk_size = std::stoul(this->input.substr(
				this->pos, crlf - this->pos), &chunk_ext_start, 16);
		} catch (const std::out_of_range &e) {
			this->setStatus(413);
			return (true);
		} catch (const std::invalid_argument &e) {
			this->setStatus(400);
			return (true);
		}
		PARSE_ASSERT(crlf > this->pos);
		//skips extensions
		this->pos = 2 + crlf;
		if (chunk_size == 0) {
			break ;
		}
		if (this->request.body->length() + chunk_size > max_body_len) {
			this->setStatus(413);
			return (true);
		}
		if (this->input.length() < chunk_size + this->pos + 2) {
			this->pos = old_pos;
			return (false);
		}
		if (this->input[this->pos + chunk_size] != '\r'
			|| this->input[this->pos + chunk_size + 1] != '\n')
		{
			this->setStatus(400);
			return (true);
		}
		if (this->request.body.has_value()) {
			this->request.body->append(this->input, this->pos, chunk_size);
		} else {
			this->request.body = this->input.substr(this->pos, chunk_size);
		}
		this->pos += chunk_size + 2;
	}
	size_t trailer_end = this->input.find("\r\n\r\n", this->pos);
	if (trailer_end == std::string::npos) {
		return (false);
	}
	//skips trailers
	this->pos = trailer_end + 4;
	return (true);
}

bool RequestParser::parse_encoded_body(size_t max_body_len) {
	std::string encoding_str = this->request.headers["transfer-encoding"];
	std::transform(encoding_str.begin(), encoding_str.end(), encoding_str.begin(), [](char c){return (std::tolower(c));});
	std::cout << "encoding: " << encoding_str << "\n";
	if (this->request.headers.find("content-length") != this->request.headers.end()) {
		std::cout << "rejecting request since content-length and encoding was found\n";
		this->setStatus(400);
		return (true);
	}
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
bool RequestParser::parse_body(size_t max_body_len) {
	if (this->request.body.has_value() || this->request.status_code.has_value()) {
		return (true);
	}
	if (this->request.headers.find("transfer-encoding") != this->request.headers.end()) {
		return (parse_encoded_body(max_body_len));
	} else if (this->request.headers.find("content-length") != this->request.headers.end()) {
		return (this->parse_not_encoded_body(max_body_len));
	} else {
		return (true);
	}
}

Request &&RequestParser::getRequest(void) {
	if (!this->request.body.has_value()) {
		this->request.body = "";
	}
	return (std::move(this->request));
}

#ifdef SOLO
const char *dummy_input =
"POST http://example.com:443/test/path/file.txt?test%20query HTTP/1.1\r\n" "Host: www.example.re\r\n"
//"POST http://[a:2:3:1]:443/test/path/file.txt?test%20query HTTP/1.1\r\n" "Host: www.example.re\r\n"
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
#endif // SOLO
