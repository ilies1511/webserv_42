#pragma once

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
//#include <serverConfig.hpp>

#ifndef DEFAULT_MAX_REQ_BODY_SIZE
# define DEFAULT_MAX_REQ_BODY_SIZE 10000000000
#endif // DEFAULT_MAX_REQ_BODY_SIZE

typedef struct Uri {
	std::string	full;
	std::string	authority;
	std::string	host;
	std::string	port;
	std::string	path;
	std::string	query;
	struct {//
		uint8_t	is_origin_form : 1;// localhost:8080/path/to/smth/?querry#useless data

		//									  | ----AUTH-----|---PATH-----|QUERRY| IGNORED
		//									  | HOST----|PORT|
		uint8_t	is_absolute_form : 1;// http://localhost:8080/path/to/smth/?querry#useless data
		uint8_t	is_authority_form : 1; // localhost:8080
		uint8_t	is_asterisk_form : 1; // *
	} form;
	Uri(void) {
		form.is_origin_form = 0;
		form.is_absolute_form = 0;
		form.is_authority_form = 0;
		form.is_asterisk_form = 0;
	}
	bool operator==(const struct Uri &other) const;
} uri;

typedef struct Request {
	std::optional<std::string>						method;
	std::optional<Uri>								uri;
	std::optional<std::string>						version;
	std::unordered_map<std::string, std::string>	headers;
	std::optional<std::string>						body;
	std::optional<int>								status_code;
	bool operator==(const Request &other) const;
	Request(void):
		method(std::nullopt),
		uri(std::nullopt),
		version(std::nullopt),
		body(std::nullopt),
		status_code(std::nullopt)
	{}
} Request;

class RequestParser {
public:
				RequestParser(void) = delete;
				RequestParser(std::string &input);
				~RequestParser();
	bool		parse_request_line(void);
	bool		parse_uri(void);
	bool		parse_headers(void);
	bool		parse_body(size_t max_body_len);
	Request		&&getRequest(void);

	Request		request;
	std::string &input;
private:

	void	setStatus(int status);
	bool	finished_request_line = false;
	bool	finished_headers = false;
	bool	finished = false;

	long					pos;
	const static std::regex	method_pat;
	const static std::regex	request_line_pat;
	const static std::regex	uri_pat;
	const static std::regex	header_name_pat;
	const static std::regex	header_value_pat;

	bool	parse_method(void);
	bool	parse_not_encoded_body(size_t max_body_len);
	bool	parse_encoded_body(size_t max_body_len);
	bool	parse_chunked(size_t max_body_len);

	/*only called by a macro */
	bool	parse_assertion_exec(bool cond, const char *str_cond,
						const char *file, const int line, const char *fn_str);
};


std::ostream& operator<<(std::ostream& output, Uri uri);
std::ostream& operator<<(std::ostream &output, const Request &request);

