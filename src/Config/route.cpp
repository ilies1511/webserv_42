#include "../../Includes/Config/route.hpp"

 route::route() : _GETOption(false),
                 _POSTOption(false),
                 _root(""),
                 _redirect(std::make_pair(0, "")),
                 _autoindex(false),
                 _index(),
                 _file_upload("") {

 };

route::~route() {};

////////////////////////////////////////////////////////////////////////////////
///////////////                  GETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////


bool route::getGETOption() const {
    return _GETOption;
}

bool route::getPOSTOption() const {
    return _POSTOption;
}

std::string route::getRoot() const {
    return _root;
}

std::pair<std::size_t, std::string> route::getRedirect() {
    return _redirect;
}

bool route::getAutoIndex() {
    return _autoindex;
}

std::vector<std::string> route::getIndex() {
    return _index;
}

std::string route::getFileUpload() {
    return _file_upload;
}

////////////////////////////////////////////////////////////////////////////////
///////////////                  SETTERS                           /////////////
////////////////////////////////////////////////////////////////////////////////


void route::setGetOption(bool option) {
    _GETOption = option;
}

void route::setPostOption(bool option) {
    _POSTOption = option;
}

void route::setRoot(const std::string &path) {
    _root = path;
}

void route::setRedirect(std::size_t errorNbr, std::string path) {
    // implementation
}

void route::setAutoIndex(bool option) {
    _autoindex = option;
}

void route::setIndex(const std::vector<std::string>& indexes) {
    _index = indexes;
}

void route::setFileUpload(const std::string &path) {
    _file_upload = path;
}

















