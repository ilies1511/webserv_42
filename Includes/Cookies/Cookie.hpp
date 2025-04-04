#ifndef Cookie_HPP
#define Cookie_HPP

#include <iostream>
#include <string>

class Cookie
{
	public:
		std::string	cookie_string;
		long		max_age;
	public:
	//OCF -- BEGIN
		Cookie(std::string  set_cookie_header);
		Cookie(const Cookie& other) = default;
		~Cookie(void);
		Cookie& operator=(const Cookie& other) = default;

	void	setMaxAge(long nbr);
	long	getMaxAge() const;
	private:
		Cookie(void) = delete;
	//OCF -- END
	//Methodes -- BEGIN

	//Methodes -- END
};

#endif
