#pragma once

#include "../../Includes/request_parser/RequestParser.hpp"
#include <cstring>

#ifndef PARSE_ASSERT
#define PARSE_ASSERT(cond) parse_assertion_exec(cond, #cond, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef REQUEST_LINE_MAX
//https://httpwg.org/specs/rfc9112.html#message.format '3. Request Line':
//'It is RECOMMENDED that all HTTP senders and recipients support, at a
//minimum, request-line lengths of 8000 octets'

# define REQUEST_LINE_MAX 8000 //if it's longer than this give a 501
#endif // REQUEST_LINE_MAX

#define METHOD_PAT "(?:^(?:(?:(GET|POST|DELETE) )|([a-zA-Z]+ )|([a-zA-Z]*$)))"

#define VERSION_PAT "(?:(^(?:(?:(HTTP\\/1\\.1)\\r)|(?:(\\w+\\/\\d+\\.\\d+)\\r))(\\n)?)((?:[\\s\\S]+)$)?)"

#ifndef URI_MAX
//todo
# define URI_MAX PATH_MAX// give a 414
#endif // URI_MAX

// request line patterns
//todo: currently the path can have anything that is not space, '?' or '#'
#define PATH "((?:\\/)|(?:(?:\\/[^\\s\\/\\?\\#\\*]+)+)|(?:(?:\\/[^\\s\\/\\?\\#]+)+\\/))"

//#define QUERY "(?:(?:\\?)([^\\s\\#]*))"
#define QUERY "(?:(?:\\?)([A-Za-z0-9\\-_\\.\\~\\%]*))"

#define URI_TERM "(?:(?:\\#\\S*$)|$)" // either end of uri or # folloed by some non whitespace

#define ORIGIN_FORM "(" PATH QUERY "?" URI_TERM ")"

#define IPV6_SEG "[0-9a-zA-Z]{1,4}"
#define IPV6_FULL "(?:(?:" IPV6_SEG "\\:){7}" IPV6_SEG ")"
//not full sure about ipv6 syntax for mixed/compressed
#define IPV6_COMPRESSED "(?:(?:" IPV6_SEG "(?::" IPV6_SEG "){0,5})?::(?:" IPV6_SEG "(?::" IPV6_SEG "){0,5})?)"
#define IPV6_MIX "(?:" IPV6_SEG "(?::" IPV6_SEG "){5}:(?:[0-9]{1,3}\\.){3}[0-9]{1,3})"
#define IPV6 "(?:\\[(" IPV6_FULL "|" IPV6_COMPRESSED "|" IPV6_MIX ")\\])"

#define HOST "((?:[\\w\\.]+)|" IPV6 ")"
#define PORT "([1-9]\\d{0,3})"

#define AUTHORITY_FORM "(^" HOST ":" PORT "$)"

//#define ABSOLUTE_FORM "(^http\\:\\/\\/" "(" HOST ":" PORT ")" ORIGIN_FORM "$)"

#define ABSOLUTE_FORM "(^http\\:\\/\\/" "(" HOST "(?:" ":" PORT ")?" ")" ORIGIN_FORM "$)"

#define ASTERISK_FORM "(\\*)" // not implemented

#define URI_PAT "(?:^" ORIGIN_FORM "|" AUTHORITY_FORM "|" ABSOLUTE_FORM "|" ASTERISK_FORM "$)"

// header patterns
#define FIELD_NAME "(?:^(?:([^\\:\\s][^\\:\\n]+)\\:)|^([^\\:\\n]*$))"
//#define FIELD_VALUE "(?:^([ \\t\\f\\v]*(\\S[^\\r\\n]*)\\r\\n)|^([ \\t\\f\\v]*(?:(?:\\S[^\\r\\n]*)|(?:\\S))?\\r?$))"
#define FIELD_VALUE "(?:^([ \\t\\f\\v]*((?:\\S[^\\r\\n]*)|(?:))\\r\\n)|^([ \\t\\f\\v]*(?:(?:\\S[^\\r\\n]*)|(?:\\S))?\\r?$))"

//#define FIELD_VALUE 


template<class T>
std::ostream& operator<<(std::ostream& output, std::optional<T> val) {
	if (val.has_value()) {
		output << *val;
	} else {
		output << "No value";
	}
	return (output);
}
