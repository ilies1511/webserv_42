#include <fcntl.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <errno.h>

std::vector<std::pair<int, std::basic_string<char>>> all =
{
	{100, "Continue"},
	{101, "Switching Protocols"},
	{102, "Processing"},
	{103, "Early Hints"},
	{200, "OK"},
	{201, "Created"},
	{202, "Accepted"},
	{203, "Non-Authoritative Information"},
	{204, "No Content"},
	{205, "Reset Content"},
	{206, "Partial Content"},
	{207, "Multi-Status"},
	{208, "Already Reported"},
	{226, "IM Used"},
	{300, "Multiple Choices"},
	{301, "Moved Permanently"},
	{302, "Found"},
	{303, "See Other"},
	{304, "Not Modified"},
	{305, "Use Proxy"},
	{306, "Switch Proxy"},
	{307, "Temporary Redirect"},
	{308, "Permanent Redirect"},
	{400, "Bad Request"},
	{401, "Unauthorized"},
	{402, "Payment Required"},
	{403, "Forbidden"},
	{404, "Not Found"},
	{405, "Method Not Allowed"},
	{406, "Not Acceptable"},
	{407, "Proxy Authentication Required"},
	{408, "Request Timeout"},
	{409, "Conflict"},
	{410, "Gone"},
	{411, "Length Required"},
	{412, "Precondition Failed"},
	{413, "Payload Too Large"},
	{414, "URI Too Long"},
	{415, "Unsupported Media Type"},
	{416, "Range Not Satisfiable"},
	{417, "Expectation Failed"},
	{418, "I'm a teapot"},
	{421, "Misdirected Request"},
	{422, "Unprocessable Content"},
	{423, "Locked"},
	{424, "Failed Dependency"},
	{425, "Too Early"},
	{426, "Upgrade Required"},
	{428, "Precondition Required"},
	{429, "Too Many Requests"},
	{431, "Request Header Fields Too Large"},
	{451, "Unavailable For Legal Reasons"},
	{500, "Internal Server Error"},
	{501, "Not Implemented"},
	{502, "Bad Gateway"},
	{503, "Service Unavailable"},
	{504, "Gateway Timeout"},
	{505, "HTTP Version Not Supported"},
	{506, "Variant Also Negotiates"},
	{507, "Insufficient Storage"},
	{508, "Loop Detected"},
	{510, "Not Extended"},
	{511, "Network Authentication Required"},
};

int main (void) {
	std::string s1 =
	"<!DOCTYPE html>\n"
	"<html lang=\"en\">\n"
	"<head>\n"
	"    <meta charset=\"UTF-8\">\n"
	"    <title>"
	;
	std::string s2 =
	"</title>\n"
	"</head>\n"
	"<body>\n"
	"    <h1>"
	;
	std::string s3 =
	"</h1>\n"
	"    <p>Oops! Something went wrong.</p>\n"
	"    <hr>\n"
	"    <p><a href=\"/\">Return to Home</a></p>\n"
	"</body>\n"
	"</html>\n"
	;
	for (auto &v : all) {
		int fd = open(std::string(std::string("./errorPages/") + std::to_string(v.first) + ".html").c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0) {
			std::cerr << strerror(errno) << std::endl;
			exit(1);
		}
		std::string data = s1 + std::to_string(v.first) + " " + v.second + s2 + std::to_string(v.first) + " - " + v.second + s3;
		if (write(fd, data.c_str(), data.length()) <= 0) {
			std::cerr << strerror(errno) << std::endl;
			exit(1);
		}
		close(fd);
	}
}
