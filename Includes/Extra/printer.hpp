/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printer.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iziane <iziane@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/21 09:49:30 by iziane            #+#    #+#             */
/*   Updated: 2025/02/09 00:49:35 by iziane           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include "Log.hpp"

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
}
