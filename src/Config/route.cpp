#include "../../Includes/Config/route.hpp"

route::route() : _allowed_methods(),
                 _root(""),
                 _redirect(std::make_pair(0, "")),
                 _autoindex(false),
                 _routeIsRedirect(false) {
                 // _index() {
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

std::vector<std::string> route::getAllowedMethods() const {
    return _allowed_methods;
}


std::string route::getRoot() const {
    return _root;
}

std::string route::getAlias() const {
    return _alias;
}

std::string route::getActualPath() const {
    return _actual_path;
}



std::pair<std::size_t, std::string> route::getRedirect() const {
    return _redirect;
}

bool route::getAutoIndex() const {
    return _autoindex;
}

std::string route::getIndex() const {
    return _index;
}

// std::string route::getFileUpload() const {
//     return _file_upload;
// }

std::unordered_map<std::string, std::string> route::getCgi() const {
    return _cgi;
}

bool route::getRouteIsRedirect() const {
    return _routeIsRedirect;
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

void route::setAllowedMethods(const std::string &option) {
    _allowed_methods.push_back(option);
}


void route::setRoot(const std::string &path) {
    _root = path;
}

void route::setAlias(const std::string &alias) {
    _alias = alias;
}

void route::setActualPath(const std::string &path) {
    _actual_path = path;
}



void route::setRedirect(const std::size_t errorNbr, const std::string &path) {
    _redirect.first = errorNbr;
    _redirect.second = path;
}

void route::setAutoIndex(bool option) {
    _autoindex = option;
}

void route::setIndex(const std::string& index) {
    _index = index;
}

// void route::setFileUpload(const std::string &path) {
//     _file_upload = path;
// }

void route::setCgi(const std::string &type, const std::string &path) {
    _cgi[type] = path;
}

void route::setRouteIsRedirect(bool option) {
    _routeIsRedirect = option;
}


// void route::PrintRoute() {
//     std::cout << "  Allowed Methods:";
//     for (const auto& method : _limit_except) {
//         std::cout << " " << method;
//     }
//     std::cout << std::endl;
//     std::cout << "  Root: " << _root << std::endl;
//     std::cout << " Alias: " << _alias << std::endl;
//     std::cout << "  Redirect: " << _redirect.first << " " << _redirect.second;
//     std::cout << "  Autoindex: ";
//     if (_autoindex == true) {
//         std::cout << "on";
//     } else {
//         std::cout << "off";
//     }
//     std::cout << std::endl;
//     std::cout << "  Index: ";
//     for (const auto& word : _index) {
//         std::cout << "  " << word;
//     }
//     std::cout << std::endl;
//     std::cout << "  CGI: " << std::endl;
//     for (const auto& [fst, snd] : _cgi) {
//         std::cout << "    " << fst << " " << snd << std::endl;
//     }
// }
std::ostream& operator<<(std::ostream& os, const route& r) {

    if (r._routeIsRedirect) {
        os << "  Redirect: " << r._redirect.first << " " << r._redirect.second << std::endl;
        os << "  [Redirect route]" << std::endl;
    } else {
        os << "  Allowed Methods:";
        if (r._allowed_methods.empty()) {
            os << " None";
        } else {
            for (const auto& method : r._allowed_methods) {
                os << " " << method;
            }
        }
        os << std::endl;
        if (!r._root.empty()) {
            os << "  Root: " << r._root << std::endl;
        }
        if (!r._alias.empty()) {
            os << "  Alias: " << r._alias << std::endl;
        }
        os << "  Actual Path: " << r._actual_path << std::endl;
        if (r._autoindex) {
            os << "  Autoindex: on" << std::endl;
        }
        if (!r._index.empty()) {
            os << "  Index: " << r._index << std::endl;
        }
        if (!r._cgi.empty()) {
            os << "  CGI: " << std::endl;
            for (const auto& [fst, snd] : r._cgi) {
                os << "    " << fst << " " << snd << std::endl;
            }
        }
        os << "  [Regular route]" << std::endl;
    }


    // os << "  Allowed Methods:";
    // for (const auto& method : r._allowed_methods) {
        // os << " " << method;
    // }
    // os << std::endl;
    // os << "  Root: " << r._root << std::endl;
    // os << "  Alias: " << r._alias << std::endl;
    // os << "  Actual Path: " << r._actual_path << std::endl;
    // os << "  Redirect: " << r._redirect.first << " " << r._redirect.second << std::endl;
    // os << "  Autoindex: ";
    // if (r._autoindex == true) {
    //     os << "on";
    // } else {
    //     os << "off";
    // }
    // os << std::endl;
    // os << "  Index: " << r._index << std::endl;
    // os << "  CGI: " << std::endl;
    // for (const auto& [fst, snd] : r._cgi) {
    //     os << "    " << fst << " " << snd << std::endl;
    // }
    // if (r._routeIsRedirect == true) {
    //     os << "  [Redirect route]" << std::endl;
    // } else {
    //     os << "  [Regular route]" << std::endl;
    // }
    return os;
}



route::route(route&& other)
    : _allowed_methods(std::move(other._allowed_methods)),
      _root(std::move(other._root)),
      _redirect(std::move(other._redirect)),
      _autoindex(other._autoindex),
      _index(std::move(other._index)),
      _alias(std::move(other._alias)),
      _actual_path(std::move(other._actual_path)),
      _cgi(std::move(other._cgi)),
      _routeIsRedirect(other._routeIsRedirect)
{
}

route& route::operator=(route&& other) {
    if (this != &other) {
        _allowed_methods = std::move(other._allowed_methods);
        _root            = std::move(other._root);
        _redirect        = std::move(other._redirect);
        _autoindex       = other._autoindex;
        _index           = std::move(other._index);
        _alias           = std::move(other._alias);
        _actual_path     = std::move(other._actual_path);
        _cgi             = std::move(other._cgi);
        _routeIsRedirect = other._routeIsRedirect;
    }
    return *this;
}

route& route::operator=(const route& other) {
    if (this != &other) {
        _allowed_methods = other._allowed_methods;
        _root            = other._root;
        _redirect        = other._redirect;
        _autoindex       = other._autoindex;
        _index           = other._index;
        _alias           = other._alias;
        _actual_path     = other._actual_path;
        _cgi             = other._cgi;
        _routeIsRedirect = other._routeIsRedirect;
    }
    return *this;
}





