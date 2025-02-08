/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iziane <iziane@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/07 21:34:08 by iziane            #+#    #+#             */
/*   Updated: 2025/01/03 12:12:22 by iziane           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "printer.hpp"

std::string coloring(const std::string& text, const std::string& color)
{
	return color + text + NC;
}

// Usage: printer::ocf_printer("Character", printer::OCF_TYPE::DC);
namespace printer
{
	void ocf_printer(const std::string& class_name, OCF_TYPE type, const std::string& color)
	{
		(void)class_name;
		(void)type;
		(void)color;
		#ifdef DEBUG
		std::string str;
		switch (type)
		{
			case OCF_TYPE::DC:	str = "[" + class_name + "]: " + "Default Constructor called"; break;
			case OCF_TYPE::DNC:	str = "[" + class_name + "]: " + "Default Name Constructor called";	break;
			case OCF_TYPE::CC:	str = "[" + class_name + "]: " + "Copy Constructor called";	break;
			case OCF_TYPE::CAC:	str = "[" + class_name + "]: " + "Copy Assignment Constructor called";	break;
			case OCF_TYPE::D:	str = "[" + class_name + "]: " + "Destructor called";	break;
		}
		std::cout << coloring(str, color) << "\n";
		#endif
	}

	void PrintMessage(const std::string& message, MessageType type)
	{
		std::string color;
		switch (type)
		{
			case MessageType::INFO:		color = PURPLE;	break;
			case MessageType::PROMPT:	color = BLUE;	break;
			case MessageType::ERROR:	color = RED;	break;
			case MessageType::HEADER:	color = ORANGE;	break;
			case MessageType::SUCCESS:	color = GREEN;	break;
			case MessageType::OCF:		color = BROWN;	break;
		}
		std::cout << coloring(message, color) << "\n";
	}

	void Header(const std::string& Input)
	{
		std::cout << coloring(Input, ORANGE) << "\n";
	}

	void print_Interactive(const std::string& Input)
	{
		std::cout << coloring(Input, BLUE);
	}

	void print_invalidInput()
	{
		std::cout << coloring(
			"[INFO]: Invalid Command. USAGE: <cat> <dog> <wronganimal> <wrongcat> <clear> <exit>",
			RED
		) << "\n";
	}

	void LogException(const std::exception& e, const char* file, const char* function, int line)
	{
		std::cerr << coloring("[Exception Caught]", YELLOW) << "\n";
		std::cerr << coloring("File: ", YELLOW) << file << "\n";
		std::cerr << coloring("Function: ", YELLOW) << function << "\n";
		std::cerr << coloring("Line: ", YELLOW) << line << "\n";
		std::cerr << coloring("Reason: ", YELLOW) << coloring(e.what(), RED) << "\n";
	}
}

