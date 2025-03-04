#include "../../Includes/Config/route.hpp"

 route::route() : _limit_except(),
                 _root(""),
                 _redirect(std::make_pair(0, "")),
                 _autoindex(false),
                 _index() {
                 // _file_upload("") {

 };

route::~route() {};

////////////////////////////////////////////////////////////////////////////////
///////////////                  GETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////


// bool route::getGETOption() const {
//     return _GETOption;
// }
//
// bool route::getPOSTOption() const {
//     return _POSTOption;
// }

std::vector<std::string> route::getLimitsExcept() const {
    return _limit_except;
}


std::string route::getRoot() const {
    return _root;
}

std::pair<std::size_t, std::string> route::getRedirect() {
    return _redirect;
}

bool route::getAutoIndex() const {
    return _autoindex;
}

std::vector<std::string> route::getIndex() const {
    return _index;
}

// std::string route::getFileUpload() const {
//     return _file_upload;
// }

std::unordered_map<std::string, std::string> route::getCgi() const {
    return _cgi;
}

////////////////////////////////////////////////////////////////////////////////
///////////////                  SETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////


// void route::setGetOption(bool option) {
//     _GETOption = option;
// }
//
// void route::setPostOption(bool option) {
//     _POSTOption = option;
// }

void route::setLimitsExcept(const std::string &option) {
    _limit_except.push_back(option);
}


void route::setRoot(const std::string &path) {
    _root = path;
}

void route::setRedirect(const std::size_t errorNbr, const std::string &path) {
    _redirect.first = errorNbr;
    _redirect.second = path;
}

void route::setAutoIndex(bool option) {
    _autoindex = option;
}

void route::setIndex(const std::string& index) {
    _index.push_back(index);
}

// void route::setFileUpload(const std::string &path) {
//     _file_upload = path;
// }

void route::setCgi(const std::string &type, const std::string &path) {
    _cgi[type] = path;
}

void route::PrintRoute() {
    std::cout << "  Allowed Methods:";
    for (const auto& method : _limit_except) {
        std::cout << " " << method;
    }
    std::cout << std::endl;
    std::cout << "  Root: " << _root << std::endl;
    std::cout << "  Redirect: " << _redirect.first << " " << _redirect.second;
    std::cout << "  Autoindex: ";
    if (_autoindex == true) {
        std::cout << "on";
    } else {
        std::cout << "off";
    }
    std::cout << std::endl;
    std::cout << "  Index: ";
    for (const auto& word : _index) {
        std::cout << "  " << word;
    }
    std::cout << std::endl;
    std::cout << "  CGI: " << std::endl;
    for (const auto& [fst, snd] : _cgi) {
        std::cout << "    " << fst << " " << snd << std::endl;
    }
}
std::ostream& operator<<(std::ostream& os, const route& r) {
    os << "  Allowed Methods:";
    for (const auto& method : r._limit_except) {
        os << " " << method;
    }
    os << std::endl;
    os << "  Root: " << r._root << std::endl;
    os << "  Redirect: " << r._redirect.first << " " << r._redirect.second << std::endl;
    os << "  Autoindex: ";
    if (r._autoindex == true) {
        os << "on";
    } else {
        os << "off";
    }
    os << std::endl;
    os << "  Index: ";
    for (const auto& word : r._index) {
        os << "  " << word;
    }
    os << std::endl;
    os << "  CGI: " << std::endl;
    for (const auto& [fst, snd] : r._cgi) {
        os << "    " << fst << " " << snd << std::endl;
    }
    return os;
}
















