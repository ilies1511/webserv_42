#pragma once
#include <string>
#include <algorithm>
#include <regex>
#include <iostream>


bool isValidPort(const std::string& port);
bool isValidIPv4(const std::string& ip);
bool isValidIPPort(const std::string& combination);