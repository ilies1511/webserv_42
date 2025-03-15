#include "../../Includes/Config/parser.hpp"

void validateLocationParams(route& current_route, const std::string& keyword, const std::vector<std::string>& params) {
    if (keyword == "index") {
        if (params.size() != 1) {
            throw std::runtime_error(" index: invalid arguments");
        }
        current_route.setIndex(params[0]);
    } if (keyword == "limit_except") {
        if (params.empty()) {
            throw std::runtime_error(" limit_except: invalid arguments");
        }
        for (const auto & param : params) {
           if (param == "POST" || param == "GET" || param == "DELETE") {
               current_route.setLimitsExcept(param);
           } else {
               throw std::runtime_error(" limit_except: invalid arguments");
           }
        }
    } if (keyword == "return") {
        // should be validated first
        if (params.size() != 2) {
            throw std::runtime_error(" return: invalid arguments");
        }
        current_route.setRedirect(stoul(params[0]), params[1]);
    } if (keyword == "autoindex") {
        if (params.size() != 1) {
            throw std::runtime_error(" autoindex: invalid arguments");
        } if (params[0] == "on") {
            current_route.setAutoIndex(true);
        } else if (params[0] == "off") {
            current_route.setAutoIndex(false);
        } else {
            throw std::runtime_error(" autoindex: invalid arguments");
        }
    } if (keyword == "root") {
        // should be validated first
        if (params.size() != 1) {
            throw std::runtime_error(" root: invalid arguments");
        }
        if (!current_route.getAlias().empty()) {
            throw std::runtime_error(" root: mixing root and alias not allowed");
        }
        current_route.setRoot(params[0]);
    } if (keyword == "cgi") {
        // should be validated first
        if (params.size() != 2) {
            throw std::runtime_error(" cgi: invalid arguments");
        }
        current_route.setCgi(params[0], params[1]);
    } if (keyword == "alias") {
        if (params.size() != 1) {
            throw std::runtime_error(" alias: invalid arguments");
        }
        if (!current_route.getRoot().empty()) {
            throw std::runtime_error(" alias: mixing root and alias not allowed");
        }
        current_route.setAlias(params[0]);
    }
}

std::size_t locationPrint(serverConfig& server_config, const std::vector<TOKEN>& tokenList, std::size_t i) {
    if (i < tokenList.size() && tokenList[i].type == PARAM ) {
        // should be a valid location path
    } else {
        throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " " + tokenList[i].content + " is not a valid URI pattern for a location");
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
                            const std::string error_message = "line: " + std::to_string(tokenList[i].line) + e.what();
                            throw std::runtime_error(error_message);
                        }
                        break;
                    } else if (tokenList[i].type == PARAM) {
                        keyword_params.push_back(tokenList[i].content);
                    } else {
                        throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " Missing Semicolon after: " + current_keyword + " value/s");
                    }
                }
            } else {
                throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " Invalid Keyword: " + current_keyword);
            }
        }
    } else {
        // "missing opening brace after location URI pattern"
        throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " missing opening brace after location URI pattern");
    }
    throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " unknown error");

    // return i;
}

// add serverConfig& server_config to add values
void validateParam(serverConfig& server_config, const std::string &keyword, const std::vector<std::string>& params) {
    if (keyword == "listen") {
        if (params.size() != 1) {
            throw std::runtime_error(" listen: invalid arguments");
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
            throw std::runtime_error(" listen: invalid arguments");
        }
        server_config.setIP(ip);
        server_config.setPort(std::stoul(port));
    } if (keyword == "server_name") {
        for (const auto& param : params) {
            if (isValidServerName(param)) {
                server_config.setServerName(param);
            } else {
                throw std::runtime_error(" server_name: invalid arguments");
            }
        }
    } if (keyword == "client_max_body_size") {
        if (params.size() != 1) {
            throw std::runtime_error(" client_max_body_size: invalid arguments");
        }
        std::size_t result;
        if (!isValidDataSize(params[0], result)) {
            throw std::runtime_error(" client_max_body_size: invalid arguments");
        }
        server_config.setClientMaxBodySize(result);
    } if (keyword == "root") {
        if (params.size() != 1) {
            throw std::runtime_error(" root: invalid arguments");
        }
        // should be validated first
        server_config.setRoot(params[0]);
    } if (keyword == "error_page") {
        if (params.size() < 2) {
            throw std::runtime_error(" error_page: invalid arguments");
        }
        for (std::size_t i = 0; i < params.size() - 1; ++i) {
            auto temp_map = server_config.getErrorPages();
            std::size_t errorNbr = std::stoul(params[i]);
            if (temp_map.find(errorNbr) != temp_map.end()) {
                throw std::runtime_error(" error_page: invalid arguments");
            }
            server_config.setErrorPages(errorNbr, params.back());
        }
    } if (keyword == "timeout") {
         if (params.size() != 1) {
             throw std::runtime_error(" timeout: invalid arguments");
         }
         double result;
         if (!isValidTime(params[0], result)) {
             throw std::runtime_error(" timeout: invalid arguments");
         }
         server_config.setTimeout(result);
    // } if (keyword == "limit_except") {
    //     for (const auto& option : params) {
    //         if (option == "GET" || option == "POST" || option == "DELETE") {
    //             server_config.setLimitsExcept(option);
    //         } else {
    //             throw std::runtime_error(" limit_except: invalid arguments");
    //         }
    //     }
    } if (keyword == "index") {
        if (params.size() != 1) {
            throw std::runtime_error(" index: invalid arguments");
        }
        server_config.setIndex(params[0]);
    }
}

void update_routes(std::vector<serverConfig>& server_configs) {
   for (auto& server_config : server_configs) {
       auto locations = server_config.getLocation();
       for (auto& [uri, Route] : locations) {
           route& temp = server_config.getRoute(uri);
           if (server_config.getRoute(uri).getRoot().empty() && server_config.getRoute(uri).getAlias().empty()) {
               std::string serverRoot = server_config.getRoot();
               // temp.setRoot(server_config.getRoot());
               temp.setRoot(serverRoot);
               temp.setActualPath(serverRoot + uri);
           } else if (!server_config.getRoute(uri).getAlias().empty()) {
               temp.setActualPath(server_config.getRoute(uri).getAlias());
           }
           if (server_config.getRoute(uri).getIndex().empty()) {
              temp.setIndex(server_config.getIndex());
           }
           if (!server_config.getRoute(uri).getRoot().empty()) {
               temp.setActualPath(server_config.getRoute(uri).getRoot() + uri);
           }
           // if (Route.getRoot().empty()) {
           //    if (!server_config.getRoot().empty()) {
           //        std::cout << "test root" << std::endl;
           //        Route.setRoot(server_config.getRoot());
           //        std::cout << "updated Root: " << Route.getRoot() << std::endl;
           //    }
           // }
           // if (Route.getLimitsExcept().empty()) {
           //    if (!server_config.getLimitsExcept().empty()) {
           //        std::vector<std::string> methods = server_config.getLimitsExcept();
           //        for (const auto& singleMethod : methods) {
           //          Route.setLimitsExcept(singleMethod);
           //        }
           //    }
           // }
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
                                       const std::string error_message = "line: " + std::to_string(tokenList[i].line) + e.what();
                                       throw std::runtime_error(error_message);
                                   }
                                   break;
                               } if (tokenList[i].type == PARAM) {
                                   keyword_params.push_back(tokenList[i].content);
                               } else {
                                   throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " Missing Semicolon after: " + current_keyword + " value/s");
                               }
                            }
                        }
                    } else {
                        throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " Invalid Keyword: " + current_keyword);
                    }
                }
            } else {
                throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " Missing opening curly brace after server");
            }
        } else {
            throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " Server Block should start with the keyword: server");
        }
    }
    update_routes(server_configs);
    return server_configs;
}
