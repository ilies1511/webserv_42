#include "Response.hpp"
#include "RequestParser.hpp"

Response::Response(void)
	:	http_version(""),
		status_code(""),
		reason_phrase(""),
		headers{},
		body(""),
		response_inzemaking(""),
		FileData(false),
		file_data(""),
		ready2send(false)
{
	printer::ocf_printer("Response", printer::OCF_TYPE::DC);
}

Response::Response(const Response& other)
	:	http_version(other.http_version),
		status_code(other.status_code),
		reason_phrase(other.reason_phrase),
		headers(other.headers),
		body(other.body),
		response_inzemaking(other.response_inzemaking),
		FileData(other.FileData),
		file_data(other.file_data),
		ready2send(other.ready2send)
{
	printer::ocf_printer("Response", printer::OCF_TYPE::CC);
}

Response::~Response(void) {
	printer::ocf_printer("Response", printer::OCF_TYPE::D);
}

Response& Response::operator=(const Response& other)
{
	printer::ocf_printer("Response", printer::OCF_TYPE::CAC);
	if (this != &other)
	{
		http_version = other.http_version;
		status_code = other.status_code;
		reason_phrase = other.reason_phrase;
		headers = other.headers;
		body = other.body;
		response_inzemaking = other.response_inzemaking;
		FileData = other.FileData;
		file_data = other.file_data;
		ready2send = other.ready2send;
	}
	return (*this);
}

