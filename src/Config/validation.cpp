#include "../../Includes/Config/validation.hpp"

/*  IP:Port
 *      already done
 *  server_name
 *      only "a-z" "A-Z" "0-9" "-" "."
 *      start and end not "." or "-"
 *  root and index
 *      ensure exists and is accessible
 *  location
 *      detect duplicate location blocks
 *  CGI
 *      check if script interpreter exists (example php-fpm, phyton etc.)
 *  client_max_body size is set correctly
 *
 *  upload
 *      directory exists and is writable
 * every server block should have a different Port
 *
 *
 */

bool isNumeric(const std::string& nbr) {
    return std::all_of(nbr.begin(), nbr.end(), ::isdigit);
}

bool checkOccurrences(const std::string& str, char value) {
    if (std::count(str.begin(), str.end(), value) == 1) {
        return true;
    }
    return false;
}

bool isValidPort(const std::string& port) {
    if (port.empty() || !isNumeric(port)) {
        return false;
    }
    try {
        const int value = std::stoi(port);
        return value >= 0 && value <= 65535;
    } catch (...) {
        return false;
    }
}

bool isValidIPv4(const std::string& ip) {
    std::regex ipv4_regex(
        "^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]?|0)\\."
        "(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]?|0)\\."
        "(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]?|0)\\."
        "(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9][0-9]?|0)$",
        std::regex_constants::ECMAScript
    );
    return std::regex_match(ip, ipv4_regex);
}

bool isValidIPPort(const std::string& combination, std::string& ip, std::string& port) {
    if (!checkOccurrences(combination, ':')) {
        return false;
    }
    std::size_t split_point = combination.find(':');
    ip = combination.substr(0, split_point);
    port = combination.substr(split_point + 1, combination.size());
    // std::cout << "IP: " << substring1 << " Port: " << substring2 << std::endl;
    if (isValidIPv4(ip) && isValidPort(port)) {
        return true;
    }
    return false;
}

bool isValidServerName(const std::string& server_name) {
    const std::regex server_name_regex(R"(^[a-zA-Z0-9]([a-zA-Z0-9-]*[a-zA-Z0-9])?(\.[a-zA-Z0-9]([a-zA-Z0-9-]*[a-zA-Z0-9])?)*$)");
    return std::regex_match(server_name, server_name_regex);
}