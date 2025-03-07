#ifndef Response_HPP
#define Response_HPP

#include <iostream>
#include <string>

#include "Request.hpp"

class Request;

class Response
{
	public:
		std::string	status_code;
		std::string	body;
	private:
		Request	_data; //Wird eingesplegt durch Constructor
	public:
	//OCF -- BEGIN
		Response(const Request &request);
		~Response(void);
	private:
		Response(const Response& other) = delete;
		Response& operator=(const Response& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN
	public:
		std::string	process_request(void);
	//Methodes -- END
};

#endif
