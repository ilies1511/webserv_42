#ifndef Request_HPP
#define Request_HPP

#include <iostream>
#include <string>
#include <unordered_map>

class Request
{
	public:
		std::string										_method;
		std::string										_uri;
		std::string										_version;
		std::unordered_map< std::string, std::string >	_headers;
		std::string										_body;
		bool											is_finished;
		bool											readFile;
		std::string										filename;
	public:
	//OCF -- BEGIN
	Request(void);
	~Request(void);
	Request(const Request& other);
	Request& operator=(const Request& other);
	private:
	//OCF -- END

	//Methodes -- BEGIN
	public:
		std::string	execute_and_generate_response(void);
	//Methodes -- END
};

#endif
