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
		std::string										status_code;
		std::string										body;
		std::unordered_map< std::string, std::string >	headers;
		bool											FileData;
		std::string										file_data;
		std::string										response_inzemaking;
	private:
		Request	_data; //Wird eingesplegt durch Constructor
	public:
	//OCF -- BEGIN
		// Response(void);
		Response(const Request &request);
		~Response(void);
		Response(const Response& other);
		Response& operator=(const Response& other);
	private:
	//OCF -- END

	//Methodes -- BEGIN
	public:
		std::string	process_request(void);
	//Methodes -- END
};

#endif
