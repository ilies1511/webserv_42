#include "../../Includes/Config/validation.hpp"

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

bool isValidIPPort(const std::string& combination) {
    if (!checkOccurrences(combination, ':')) {
        return false;
    }
    std::string substring1, substring2;
    std::size_t split_point = combination.find(':');
    substring1 = combination.substr(0, split_point);
    substring2 = combination.substr(split_point+1, combination.size());
    // std::cout << "IP: " << substring1 << " Port: " << substring2 << std::endl;
    if (isValidIPv4(substring1) && isValidPort(substring2)) {
        return true;
    }
    return false;
}
