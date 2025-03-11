#pragma once

#include "../Config/serverConfig.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct requestLine {
  std::string method;
  std::string uri;
  std::string version;
};

class requestParser {
    std::vector<serverConfig>                          _serverConfigs;

    std::size_t                                        _startPos;
    std::size_t                                        _endPos;
    std::string                                        _currentLine;

    const std::string                                  _requestStr;
    requestLine                                        _requestLine;
    // std::unordered_map<std::string, std::string>    _field_line;

  public:
    explicit requestParser(std::string requestStr, const std::vector<serverConfig>& configs);
    ~requestParser();

    std::vector<std::string> split();

    void parseMessage();
    void parseHeader();
    // void parseHeader(const std::string& line);
    void parseRequestLine();
    void parseRequestFields(const std::string& );
};