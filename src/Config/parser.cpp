#include "../../Includes/Config/parser.hpp"

void validateLocationParams(route& current_route, const std::string& keyword, std::vector<std::string>& params) {
    if (keyword == "index") {
        if (params.size() < 1) {
            std::cout << "(invalid)";
            return;
        }
        for (std::size_t i = 0; i < params.size(); ++i) {
            current_route.setIndex(params[i]);
        }
        std::cout << "(valid)";

    } if (keyword == "limit_except") {
        if (params.size() < 1) {
            std::cout << "(invalid)";
            return;
        }
        for (std::size_t i = 0; i < params.size(); ++i) {
           if (params[i] == "POST" || params[i] == "GET" || params[i] == "DELETE") {
               current_route.setLimitsExcept(params[i]);
           } else {
               std::cout << "(invalid)";
               return;
           }
        }
        std::cout << "(valid)";
    } if (keyword == "return") {
        // should be validated first
        if (params.size() != 2) {
            std::cout << "(invalid)";
            return;
        }
        std::cout << "(valid)";
        current_route.setRedirect(stoul(params[0]), params[1]);
    } if (keyword == "autoindex") {
        if (params.size() != 1) {
            std::cout << "(invalid)";
            return ;
        } if (params[0] == "on") {
            current_route.setAutoIndex(true);
        } else if (params[0] == "off") {
            current_route.setAutoIndex(false);
        } else {
            std::cout << "(invalid)";
            return;
        }
        std::cout << "(valid)";

    } if (keyword == "root") {
        // should be validated first
        if (params.size() != 1) {
            std::cout << "(invalid)";
            return;
        }
        current_route.setRoot(params[0]);
        std::cout << "valid";

    } if (keyword == "cgi") {
        // should be validated first
        if (params.size() != 2) {
            std::cout << "(invalid)";
            return;
        }
        current_route.setCgi(params[0], params[1]);
        std::cout << "(valid)";
    }
}

std::size_t locationPrint(serverConfig& server_config, const std::vector<TOKEN>& tokenList, std::size_t i) {
    std::cout << "location: ";
    if (i < tokenList.size() && tokenList[i].type == PARAM ) {
        std::cout << tokenList[i].content << std::endl;;
    } else {
        std::cout << "(invalid)" << std::endl;
        return i;
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
            if (tokenList[i].type == KEYWORD) {
                std::string current_keyword = tokenList[i].content;
                // std::cout << "  " << tokenList[i].content << ": ";
                std::cout << "  " << current_keyword << ": ";
                i++;
                std::vector<std::string> keyword_params;
                for ( ; i < tokenList.size(); ++i) {
                    if (tokenList[i].type == SEMICOLON) {
                        validateLocationParams(current_route, current_keyword, keyword_params);
                        std::cout << std::endl;
                        break;
                    } else if (tokenList[i].type == PARAM) {
                        std::cout << tokenList[i].content << " ";
                        keyword_params.push_back(tokenList[i].content);
                    } else {
                        std::cout << "Config file invalid" << std::endl;
                        break;
                    }
                }
            } else {
                std::cout << "Config file invalid" << std::endl;
            }
        }
    } else {
        std::cout << "Config file invalid" << std::endl;
    }
    std::cout << "Config file invalid" << std::endl;

    return i;
}

// add serverConfig& server_config to add values
void validateParam(serverConfig& server_config, const std::string &keyword, std::vector<std::string> &params) {
    if (keyword == "listen") {
        std::string ip;
        std::string port;
        if (params.size() != 1) {
            std::cout << "(invalid)";
            return;
        }
        if (isValidPort(params[0])) {
            std::cout << "(valid Port) ";
            ip = "0.0.0.0";
            port = params[0];
        } else if (isValidIPv4(params[0])) {
            std::cout << "(valid IP) ";
            ip = params[0];
            port = "80";
        } else if (isValidIPPort(params[0], ip, port)) {
            std::cout << "(valid Port:IP) ";
        } else {
            // program should stop with error
            std::cout << "(invalid) ";
            return ;
        }
        server_config.setIP(ip);
        server_config.setPort(std::stoul(port));
    } if (keyword == "server_name") {
        for (const auto& param : params) {
            if (isValidServerName(param)) {
                server_config.setServerName(param);
            } else {
                std::cout << ("invalid");
                return;
            }
        }
        std::cout << "(valid)";
    } if (keyword == "client_max_body_size") {
        if (params.size() != 1) {
            std::cout << "(invalid)";
            return;
        }
        // should be validated first
        server_config.setClientMaxBodySize(std::stoul(params[0]));
        std::cout << "(valid)";
    } if (keyword == "root") {
        if (params.size() != 1) {
            std::cout << "(invalid)";
            return;
        }
        server_config.setRoot(params[0]);
        std::cout << "(valid)";
    } if (keyword == "error_page") {
        if (params.size() < 2) {
            std::cout << "(invalid)";
            return;
        }
        // should be validated first
        for (std::size_t i = 0; i < params.size() - 1; ++i) {
            auto temp_map = server_config.getErrorPages();
            std::size_t errorNbr = std::stoul(params[i]);
            if (temp_map.find(errorNbr) != temp_map.end()) {
                std::cout << "(invalid)";
                return;
            }
            server_config.setErrorPages(errorNbr, params.back());
        }
        std::cout << "(valid)";

    // } if (keyword == "location") {
    //     // should be redundant
    //     // add all value;
    } if (keyword == "limit_except") {
        for (const auto& option : params) {
            if (option == "GET" || option == "POST" || option == "DELETE") {
                server_config.setLimitsExcept(option);
            }
            else {
                std::cout << "(invalid)";
                return ;
            }
            std::cout << "(valid)";
        }
        // add all value;
    // } if (keyword == "index") {
    //     if (params.size() != 1) {
    //         std::cout << "(invalid)";
    //         return;
    //     }
    //     // add all value;
    // } if (keyword == "return") {
    //     // add all value;
    }
}

std::vector<serverConfig> parsing(const std::vector<TOKEN>& tokenList) {
    std::vector<serverConfig> server_configs;

    for (std::size_t i = 0; i < tokenList.size(); ++i) {
        if (tokenList[i].type == KEYWORD && tokenList[i].content == "server" ) {
            std::cout << "\nSERVER INSTANCE:" << std::endl;
            i++;
            if (i < tokenList.size() && tokenList[i].type == LBRACE) {
                server_configs.emplace_back();
                i++;
                for (; i < tokenList.size(); ++i) {
                    if (tokenList[i].type == RBRACE) {
                        break;
                    }
                    if (tokenList[i].type == KEYWORD) {
                        std::string current_keyword = tokenList[i].content;
                        if (tokenList[i].content == "location") {
                            i++;
                            i = locationPrint(server_configs.back() ,tokenList, i);
                        } else {
                            std::cout << tokenList[i].content << ": ";
                            i++;
                            std::vector<std::string> keyword_params;
                            for (; i < tokenList.size(); ++i) {
                               if (tokenList[i].type == SEMICOLON) {
                                   validateParam(server_configs.back(),current_keyword, keyword_params);
                                   std::cout << std::endl;
                                   break;
                               } if (tokenList[i].type == PARAM) {
                                   std::cout << tokenList[i].content << " ";
                                   // validateParam(server_configs.back(),current_keyword, tokenList[i].content);
                                   keyword_params.push_back(tokenList[i].content);
                               } else {
                                   std::cout << "Config file invalid1" << std::endl;
                                   break;
                               }
                            }
                        }

                    } else {
                        std::cout << "Config file invalid2" << std::endl;
                    }
                }
            } else {
                std::cout << "Config file invalid3" << std::endl;
            }
        } else {
            std::cout << "Config file invalid4" << std::endl;
        }
    }
    return server_configs;
}
