#ifndef Response_HPP
#define Response_HPP

#include <iostream>
#include <string>
#include <unordered_map>

#include "Request.hpp"

class Request;

class Response
{
	public:
		std::string										http_version;// z.B. "HTTP/1.1"
		std::string										status_code;
		std::string										reason_phrase;// z.B. "OK"
		std::unordered_map< std::string, std::string >	headers;
		std::string										body;

		//Assembled Response
		std::string										response_inzemaking;

		bool											FileData;
		std::string										file_data;
		bool											ready2send;
	private:
	public:
	//OCF -- BEGIN
		// Response(void);
		Response(void);
		// Response(const Request &request);
		~Response(void);
		Response(const Response& other);
		Response& operator=(const Response& other);
	private:
	//OCF -- END

	//Methodes -- BEGIN
	public:
		// std::string	process_request(void);
		std::string	process_request(Request &request);
	//Methodes -- END
};

#endif
