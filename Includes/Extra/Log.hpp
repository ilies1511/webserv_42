#pragma once

//Includes
#include <iostream>
#include <string>

// ANSI Escape Codes
const std::string GREEN = "\033[0;32m";
const std::string RED = "\033[0;31m";
const std::string BLUE = "\033[0;34m";
const std::string BOLD = "\033[1m";
const std::string ORANGE = "\033[38;2;255;165;0m";
const std::string PURPLE = "\033[38;2;128;0;128m";
const std::string BROWN = "\033[38;2;165;42;42m";
const std::string TURQUOISE = "\033[38;2;64;224;208m";
const std::string OLIVE = "\033[38;2;128;128;0m";
const std::string BLACK = "\033[0;30m";
const std::string WHITE = "\033[0;37m";
const std::string YELLOW = "\033[0;33m";
const std::string CYAN = "\033[0;36m";
const std::string MAGENTA = "\033[0;35m";
const std::string LIGHT_GRAY = "\033[0;37m";
const std::string DARK_GRAY = "\033[1;30m";
const std::string LIGHT_RED = "\033[1;31m";
const std::string LIGHT_GREEN = "\033[1;32m";
const std::string LIGHT_BLUE = "\033[1;34m";
const std::string LIGHT_CYAN = "\033[1;36m";
const std::string LIGHT_MAGENTA = "\033[1;35m";
const std::string NC = "\033[0m"; // Reset

//Class
class Log
{
	//Attributes
	private:
		const std::string _programm;
	//Private Member-FNCs
	private:
		void logMessage(const std::string& level, const std::string& color, \
			const char* file, const char* function, int line, const std::string& msg);
		void debug(const char* file, const char* function, int line, const std::string& msg);
		void info(const char* file, const char* function, int line, const std::string& msg);
		void warning(const char* file, const char* function, int line, const std::string& msg);
		void error(const char* file, const char* function, int line, const std::string& msg);
	public:
		//OCF
		Log(void);
		Log(const std::string programm);
		Log(const Log &og);
		Log &operator=(const Log &og);
		~Log(void);
		//Members
	public:
		void complain( std::string level, const std::string msg = NULL, const char* file = NULL, const char* function = NULL, int line = 0);
};
