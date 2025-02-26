#include "../../Includes/Config/serverConfig.hpp"
//#include "webserv.hpp"

serverConfig::serverConfig(const size_t port) : _port(port) {
    _server_name.emplace_back("127.0.0.1");
    _error_pages[404] = "404.html";
    _client_max_body_size = 1000;

    _GETOption = false;
    _POSTOption = false;
    _root = "configFiles";
};

serverConfig::~serverConfig() = default;

////////////////////////////////////////////////////////////////////////////////
///////////////                  GETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////

std::size_t serverConfig::getPort() const {
    return _port;
}

std::vector<std::string> serverConfig::getServerName() const {
    return _server_name;
}

std::map<size_t, std::string> serverConfig::getErrorPages() const {
    return _error_pages;
}

std::size_t serverConfig::getClientMaxBodySize() const {
    return _client_max_body_size;
}

bool serverConfig::getGETOption() const {
    return _GETOption;
}

bool serverConfig::getPostOption() const {
    return _POSTOption;
}

std::string serverConfig::getRoot() const {
    return _root;
}

std::vector<route> serverConfig::getLocation() const {
    return _location;
}

////////////////////////////////////////////////////////////////////////////////
///////////////                  SETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////


void serverConfig::setPort(std::size_t port) {
    _port = port;
}

void serverConfig::setServerName(const std::vector<std::string>& serverName) {
    _server_name = serverName;
}

void serverConfig::setErrorPages(std::size_t errorNbr, const std::string &path) {
    // implementation
}

void serverConfig::setClientMaxBodySize(std::size_t size) {
    _client_max_body_size = size;
}

void serverConfig::setGETOption(bool option) {
    _GETOption = option;
}

void serverConfig::setPOSTOption(bool option) {
    _POSTOption = option;
}

void serverConfig::setRoot(std::string &path) {
    _root = path;
}

void serverConfig::setLocation(route Route) {
    // implementation
}









