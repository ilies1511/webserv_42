// #pragma once
#ifndef PRINTER_HPP
#define PRINTER_HPP

#include <iostream>
#include <string>
#include "Log.hpp"

#ifndef P_DEBUG
# define P_DEBUG(content) printer::macro_debug_putstr(content, __FILE__, __FUNCTION__, __LINE__)
#endif

// Makro für Debug-Ausgaben mit expliziter Farbangabe
#ifndef P_DEBUGC
# define P_DEBUGC(msg, col) printer::macro_debug_putstr_full(msg, __FILE__, __FUNCTION__, __LINE__, col)
#endif

// Prototyp der Coloring-Funktion
std::string coloring(const std::string& text, const std::string& color);

// Forward Declaration

namespace printer
{
	enum class MessageType { INFO, PROMPT, ERROR, HEADER, SUCCESS, OCF};

	/*
		DC = Default Constructor
		DNC = Default Name Constructor
		CC = Copy Constructor
		CC = Copy Assigment Constructor
		D = Destructor
	*/
	enum class OCF_TYPE {DC, DNC, CC, CAC, D};

	void ocf_printer(const std::string& class_name, OCF_TYPE type, const std::string& color = BROWN);

	void PrintMessage(const std::string& message, MessageType type = MessageType::INFO);

	void Header(const std::string& Input);

	void print_Interactive(const std::string& Input);

	void print_invalidInput();

	//Exception Printer (try catch)
	void LogException(const std::exception& e, const char* file, const char* function, int line);

	void debug_putstr(const char* msg, const char* file, const char* function, int line);

	// void macro_debug_putstr(const char* msg, const char* file, const char* function, int line);
	void macro_debug_putstr(const char* msg, const char* file, \
		const char* function, int line, const std::string& color = TURQUOISE);

	void macro_debug_putstr_full(const char* msg, const char* file, \
		const char* function, int line, const std::string& color);
}

#endif
