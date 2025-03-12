#include "../../Includes/Config/serverConfig.hpp"
//#include "webserv.hpp"

// serverConfig::serverConfig(const size_t port) : _port(port) {
//     _server_name.emplace_back("127.0.0.1");
//     _error_pages[404] = "404.html";
//     _client_max_body_size = 1000;
//
//     _GETOption = false;
//     _POSTOption = false;
//     _root = "configFiles";
// };
serverConfig::serverConfig() : _timeout(75) {};

serverConfig::~serverConfig() = default;

////////////////////////////////////////////////////////////////////////////////
///////////////                  GETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////

std::string serverConfig::getIP() const {
    return _ip;
}

std::size_t serverConfig::getPort() const {
    return _port;
}

std::vector<std::string> serverConfig::getServerName() const {
    return _server_name;
}

std::unordered_map<size_t, std::string> serverConfig::getErrorPages() const {
    return _error_pages;
}

std::size_t serverConfig::getClientMaxBodySize() const {
    return _client_max_body_size;
}


std::vector<std::string> serverConfig::getLimitsExcept() const {
    return _limit_except;
}


std::string serverConfig::getRoot() const {
    return _root;
}

double serverConfig::getTimeout() const {
    return _timeout;
}


// std::vector<route> serverConfig::getLocation() const {
//     return _location;
// }

std::unordered_map<std::string, route> serverConfig::getLocation() const {
   return _location;
}
route serverConfig::getRoute(const std::string& Route) const {
    const auto it = _location.find(Route);
    if (it == _location.end()) {
        throw std::runtime_error("404 Not Found");
    }
   return it->second;
}

////////////////////////////////////////////////////////////////////////////////
///////////////                  SETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////

void serverConfig::setIP(const std::string& ip) {
    _ip = ip;
}

void serverConfig::setPort(std::size_t port) {
    _port = port;
}

void serverConfig::setServerName(const std::string& serverName) {
    _server_name.push_back(serverName);
    // _server_name.emplace_back(serverName);
}

void serverConfig::setErrorPages(const std::size_t errorNbr, const std::string &path) {
    _error_pages[errorNbr] = path;
}

void serverConfig::setClientMaxBodySize(std::size_t size) {
    _client_max_body_size = size;
}


void serverConfig::setLimitsExcept(const std::string& option) {
    _limit_except.push_back(option);
}

void serverConfig::setRoot(const std::string &path) {
    _root = path;
}

void serverConfig::setTimeout(const double value) {
    _timeout = value;
}


void serverConfig::setLocation(const std::string& path, const route& Route) {
    _location[path] = Route;
}

void serverConfig::printData() {
    std::cout << "IP: " << _ip << "     " << "Port: " << _port << std::endl;
    std::cout << "Server Name:";
    for (const auto& name : _server_name) {
        std::cout << " " << name;
    }
    std::cout << std::endl;
    std::cout << "Error Pages:" << std::endl;;
    for (const auto&[fst, snd] : _error_pages) {
        std::cout << "  " << fst << " " << snd << std::endl;
    }
    std::cout << "Client max body size: " << _client_max_body_size << " bytes" << std::endl;
    std::cout << "Timeout: " << _timeout << " seconds" <<std::endl;
    std::cout << "Allowed methods: ";
    for (const auto& method : _limit_except) {
        std::cout << " " << method;
    }
    std::cout << std::endl;
    std::cout << "Root: " << _root << std::endl;
    for (const auto& [fst, snd] : _location) {
        std::cout << "Location: " << fst << std::endl;
        std::cout << snd << std::endl;
    }

}








