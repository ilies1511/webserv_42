#include "../../Includes/Config/parser.hpp"

void validateLocationParams(route& current_route, const std::string& keyword, std::vector<std::string>& params) {
    if (keyword == "index") {
        if (params.empty()) {
            throw std::runtime_error("index: invalid arguments");
        }
        for (const auto & param : params) {
            current_route.setIndex(param);
        }
    } if (keyword == "limit_except") {
        if (params.empty()) {
            throw std::runtime_error("limit_except: invalid arguments");
        }
        for (const auto & param : params) {
           if (param == "POST" || param == "GET" || param == "DELETE") {
               current_route.setLimitsExcept(param);
           } else {
               throw std::runtime_error("limit_except: invalid arguments");
           }
        }
    } if (keyword == "return") {
        // should be validated first
        if (params.size() != 2) {
            throw std::runtime_error("return: invalid arguments");
        }
        current_route.setRedirect(stoul(params[0]), params[1]);
    } if (keyword == "autoindex") {
        if (params.size() != 1) {
            throw std::runtime_error("autoindex: invalid arguments");
        } if (params[0] == "on") {
            current_route.setAutoIndex(true);
        } else if (params[0] == "off") {
            current_route.setAutoIndex(false);
        } else {
            throw std::runtime_error("autoindex: invalid arguments");
        }
    } if (keyword == "root") {
        // should be validated first
        if (params.size() != 1) {
            throw std::runtime_error("root: invalid arguments");
        }
        current_route.setRoot(params[0]);
    } if (keyword == "cgi") {
        // should be validated first
        if (params.size() != 2) {
            throw std::runtime_error("cgi: invalid arguments");
        }
        current_route.setCgi(params[0], params[1]);
    }
}

std::size_t locationPrint(serverConfig& server_config, const std::vector<TOKEN>& tokenList, std::size_t i) {
    if (i < tokenList.size() && tokenList[i].type == PARAM ) {
        // should be a valid location path
    } else {
        throw std::runtime_error(tokenList[i].content + " is not a valid path of a location");
    }
    const std::string current_location = tokenList[i].content;
    i++;
    if (tokenList[i].type == LBRACE) {
        i++;
        route current_route;
        for (; i < tokenList.size(); ++i) {
            if (tokenList[i].type == RBRACE) {
                server_config.setLocation(current_location, current_route);
                return i;
            }
            std::string current_keyword = tokenList[i].content;
            if (tokenList[i].type == KEYWORD) {
                i++;
                std::vector<std::string> keyword_params;
                for ( ; i < tokenList.size(); ++i) {
                    if (tokenList[i].type == SEMICOLON) {
                        try {
                            validateLocationParams(current_route, current_keyword, keyword_params);
                        } catch (const std::exception& e) {
                            throw ;
                        }
                        break;
                    } else if (tokenList[i].type == PARAM) {
                        keyword_params.push_back(tokenList[i].content);
                    } else {
                        throw std::runtime_error("Missing Semicolon after: " + current_keyword);
                    }
                }
            } else {
                throw std::runtime_error("Invalid Keyword: " + current_keyword);
            }
        }
    } else {
        throw std::runtime_error("Config file invalid");
    }
    throw std::runtime_error("Config file invalid");

    // return i;
}

// add serverConfig& server_config to add values
void validateParam(serverConfig& server_config, const std::string &keyword, const std::vector<std::string>& params) {
    if (keyword == "listen") {
        if (params.size() != 1) {
            throw std::runtime_error("listen: invalid arguments");
        }
        std::string ip;
        std::string port;
        if (isValidPort(params[0])) {
            ip = "0.0.0.0";
            port = params[0];
        } else if (isValidIPv4(params[0])) {
            ip = params[0];
            port = "80";
        } else if (isValidIPPort(params[0], ip, port)) {
        } else {
            throw std::runtime_error("listen: invalid arguments");
        }
        server_config.setIP(ip);
        server_config.setPort(std::stoul(port));
    } if (keyword == "server_name") {
        for (const auto& param : params) {
            if (isValidServerName(param)) {
                server_config.setServerName(param);
            } else {
                throw std::runtime_error("server_name: invalid arguments");
            }
        }
    } if (keyword == "client_max_body_size") {
        if (params.size() != 1) {
            throw std::runtime_error("client_max_body_size: invalid arguments");
        }
        std::size_t result;
        if (!isValidDataSize(params[0], result)) {
            throw std::runtime_error("client_max_body_size: invalid arguments");
        }
        server_config.setClientMaxBodySize(result);
    } if (keyword == "root") {
        if (params.size() != 1) {
            throw std::runtime_error("root: invalid arguments");
        }
        // should be validated first
        server_config.setRoot(params[0]);
    } if (keyword == "error_page") {
        if (params.size() < 2) {
            throw std::runtime_error("error_page: invalid arguments");
        }
        for (std::size_t i = 0; i < params.size() - 1; ++i) {
            auto temp_map = server_config.getErrorPages();
            std::size_t errorNbr = std::stoul(params[i]);
            if (temp_map.find(errorNbr) != temp_map.end()) {
                throw std::runtime_error("error_page: invalid arguments");
            }
            server_config.setErrorPages(errorNbr, params.back());
        }
    } if (keyword == "timeout") {
         if (params.size() != 1) {
             throw std::runtime_error("timeout: invalid arguments");
         }
         double result;
         if (!isValidTime(params[0], result)) {
             throw std::runtime_error("timeout: invalid arguments");
         }
         server_config.setTimeout(result);
    } if (keyword == "limit_except") {
        for (const auto& option : params) {
            if (option == "GET" || option == "POST" || option == "DELETE") {
                server_config.setLimitsExcept(option);
            } else {
                throw std::runtime_error("limit_except: invalid arguments");
            }
        }
    }
}

std::vector<serverConfig> parsing(const std::vector<TOKEN>& tokenList) {
    std::vector<serverConfig> server_configs;

    for (std::size_t i = 0; i < tokenList.size(); ++i) {
        if (tokenList[i].type == KEYWORD && tokenList[i].content == "server" ) {
            i++;
            if (i < tokenList.size() && tokenList[i].type == LBRACE) {
                server_configs.emplace_back();
                i++;
                for (; i < tokenList.size(); ++i) {
                    if (tokenList[i].type == RBRACE) {
                        break;
                    }
                    std::string current_keyword = tokenList[i].content;
                    if (tokenList[i].type == KEYWORD) {
                        // std::string current_keyword = tokenList[i].content;
                        if (tokenList[i].content == "location") {
                            i++;
                            try {
                                i = locationPrint(server_configs.back() ,tokenList, i);
                            } catch (const std::exception& e) {
                                throw;
                            }
                        } else {
                            i++;
                            std::vector<std::string> keyword_params;
                            for (; i < tokenList.size(); ++i) {
                               if (tokenList[i].type == SEMICOLON) {
                                   try {
                                       validateParam(server_configs.back(),current_keyword, keyword_params);
                                   } catch (const std::exception& e) {
                                       throw;
                                   }
                                   break;
                               } if (tokenList[i].type == PARAM) {
                                   keyword_params.push_back(tokenList[i].content);
                               } else {
                                   throw std::runtime_error("Missing Semicolon after: " + current_keyword);
                               }
                            }
                        }
                    } else {
                        throw std::runtime_error("Invalid Keyword: " + current_keyword);
                    }
                }
            } else {
                throw std::runtime_error("Missing opening curly brace after server");
            }
        } else {
            throw std::runtime_error("Server Block should start with the keyword: server");
        }
    }
    return server_configs;
}
