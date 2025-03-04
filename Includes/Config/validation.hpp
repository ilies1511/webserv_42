#pragma once
#include <string>
#include <algorithm>
#include <regex>
#include <iostream>

#define MaxDataSize  50000000000 // 50GB
#define MaxTimeout  86400 // 24hours

/*  IP:Port done
 *
 *  server_name done
 *      only "a-z" "A-Z" "0-9" "-" "."
 *      start and end not "." or "-"
 *  root and index
 *      ensure exists and is accessible
 *  location
 *      detect duplicate location blocks - maybe also relinking
 *  CGI
 *      check if script interpreter exists (example php-fpm, phyton etc.)
 *  client_max_body size is set correctly
 *
 *  upload
 *      directory exists and is writable
 *  every server block should have a different Port
 *
 *  timeout done
 *  client_max_body_size done
 */

bool isValidPort(const std::string& port);
bool isValidIPv4(const std::string& ip);
bool isValidIPPort(const std::string& combination, std::string& ip, std::string& port);
bool isValidServerName(const std::string& server_name);
bool isValidTime(const std::string& time, double& result);
bool isValidDataSize(const std::string& data, std::size_t& result);