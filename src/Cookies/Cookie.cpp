#include "Cookie.hpp"

#include <utility>

// Cookie::Cookie(const std::string& set_cookie_header)
// {
	// (void)set_cookie_header;
// }
Cookie::Cookie(std::string  set_cookie_header)	: cookie_string(std::move(set_cookie_header)),
														max_age(0)
{

}

Cookie::~Cookie(void) {}

void Cookie::setMaxAge(long nbr) {
	max_age = nbr;
}

long Cookie::getMaxAge() const {
	return max_age;
}

