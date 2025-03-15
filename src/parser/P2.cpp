#include <istream>
#include <string>
#include <optional>
#include <unordered_map>
#include <regex>
#include <iostream>

typedef struct Response {
	std::optional<std::string>						method = std::nullopt;
	std::optional<std::string>						uri = std::nullopt;
	std::optional<std::string>						version = std::nullopt;
	std::unordered_map<std::string, std::string>	headers;
	bool											header_term = false;
	std::string										body;
	std::optional<int>								status_code = std::nullopt;
	bool											finished = false;
} Response;

template<class T>
static std::ostream& operator<<(std::ostream& output, std::optional<T> val) {
	if (val.has_value()) {
		output << val;
	} else {
		output << "No value";
	}
	return (output);
}

std::ostream& operator<<(std::ostream &output, const Response &response) {
	output << "Method: " << response.method << "\n";
	output << "URI: " << response.uri << "\n";
	output << "Version: " << response.version << "\n";
	output << "Headers:\n";
	for (const auto &header : response.headers) {
		output << "\t" << header.first << ": " << header.second << "/n";
	}
	output << "Header termination: " << response.header_term << "\n";
	output << "Body: " << response.body << "\n";
	output << "Stutus code: " << response.status_code << "\n";
	output << "Finished: " << response.finished << "\n";
	return (output);
}

class Parser {
public:
				Parser(void) = delete;
				Parser(std::string &input);
				~Parser();
	Response	parse(void);
private:
	std::string &input;
	size_t		pos;
	const static std::regex line_term;
};

const std::regex Parser::line_term(R"(\r\n)");


Parser::Parser(std::string& input):
	input(input),
	pos(0)
{
}

Parser::~Parser(void){
}

Response Parser::parse(void) {
	Response response;
	

	if (!response.method.has_value()) {
		
	}

	// validate body size
	return (response);
}

int main(int ac, char **av) {
	Response re;
	std::cout << "Empty:\n" << re;
}
