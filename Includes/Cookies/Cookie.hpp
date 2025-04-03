#ifndef Cookie_HPP
#define Cookie_HPP

#include <iostream>
#include <string>

class Cookie
{
	public:
		std::string	cookie_string;
	public:
	//OCF -- BEGIN
		Cookie(const std::string& set_cookie_header);
		Cookie(const Cookie& other) = default;
		~Cookie(void);
		Cookie& operator=(const Cookie& other) = default;
	private:
		Cookie(void) = delete;
	//OCF -- END
	//Methodes -- BEGIN

	//Methodes -- END
};

#endif
