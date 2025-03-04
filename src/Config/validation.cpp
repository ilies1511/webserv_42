#include "../../Includes/Config/validation.hpp"

#include <unordered_map>

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
 *  timeout done
 *  client_max_body_size done
 */

bool isNumeric(const std::string& nbr) {
    return std::all_of(nbr.begin(), nbr.end(), ::isdigit);
}

bool checkOccurrences(const std::string& str, const char value) {
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

bool isValidTime(const std::string& time, double& result) {
    if (time.empty()) return false;

    static const std::unordered_map<std::string, double> conversion = {
        {"ms", 0.001},
        {"s", 1},
        {"m", 60},
        {"h", 3600}
    };

    std::size_t pos = 0;
    while (pos < time.size() && std::isdigit(time[pos])) pos++;

    if (pos == time.size()) {
        result = std::stod(time);
        return true;
    }

    const std::string value = time.substr(0, pos);
    const std::string unit = time.substr(pos);

    const auto it = conversion.find(unit);
    if (it == conversion.end()) return false;

    try {
        result = std::stod(value) * it->second;
        if (result > MaxTimeout) return false;
        return true;
    } catch (...) {
        return false;
    }
}

bool isValidDataSize(const std::string& data, std::size_t& result) {
    if (data.empty()) return false;

    std::size_t pos = 0;
    while (pos < data.size() && std::isdigit(data[pos])) pos++;

    if (pos == data.size()) {
        try {
            result = std::stoul(data);
            return result <= MaxDataSize;
        } catch (...) {
            return false;
        }
    }
    if (pos > 0 && pos < data.size()) {
        const char typeSpecifier = static_cast<char>(std::toupper(data.back()));
        const std::string value = data.substr(0, pos);
        std::size_t raw = 0;
        try {
            raw = std::stoul(value);
        } catch (...) {
            return false;
        }
        switch (typeSpecifier) {
            case 'K':
                result = raw * 1024;
                break;
            case 'M':
                result = raw * 1024 * 1024;
                break;
            case 'G':
                result = raw * 1024 * 1024 * 1024;
                break;
            default:
                return false;
        }
        if (result > MaxDataSize) return false;
        return true;
    }
    return false;

}


