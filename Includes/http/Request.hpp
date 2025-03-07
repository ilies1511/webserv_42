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

	public:
	//OCF -- BEGIN
		Request(void);
		~Request(void);
	private:
		Request(const Request& other) = delete;
		Request& operator=(const Request& other) = delete;
	//OCF -- END

	//Methodes -- BEGIN
	//Methodes -- END
};

#endif
