#include "../../Includes/Config/parser.hpp"


std::size_t locationPrint(const std::vector<TOKEN>& tokenList, std::size_t i) {
    std::cout << "location: ";
    if (i < tokenList.size() && tokenList[i].type == PARAM ) {
        std::cout << tokenList[i].content << std::endl;;
    }
    i++;
    if (tokenList[i].type == LBRACE) {
        i++;
        for (; i < tokenList.size(); ++i) {
            if (tokenList[i].type == RBRACE) {
                return i;
            }
            if (tokenList[i].type == KEYWORD) {
                std::cout << "  " << tokenList[i].content << ": ";
                i++;
                for (; i < tokenList.size(); ++i) {
                    if (tokenList[i].type == SEMICOLON) {
                        std::cout << std::endl;
                        break;
                    } else if (tokenList[i].type == PARAM) {
                        std::cout << tokenList[i].content << " ";
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

void validateParam(serverConfig& server_config, const std::string& keyword, const std::string& param) {
    if (keyword == "listen") {
        if (isValidPort(param)) {
            std::cout << "listen: Port syntax is valid" << std::endl;
            //has to be changed!!!!
            server_config.setPort(80);
        } else if (isValidIPv4(param)) {
            std::cout << "listen: IP syntax is valid" << std::endl;
        } else if (isValidIPPort(param)) {
            std::cout << "listen: IP:Port syntax is valid" << std::endl;
        } else {
            std::cout << "listen: INVALID" << std::endl;
        }
        // add all value;
    } if (keyword == "server_name") {
        // add all value;
    } if (keyword == "client_max_body_size") {
        // add all value;
    } if (keyword == "root") {
        // add all value;
    } if (keyword == "error_page") {
        // add all value;
    } if (keyword == "location") {
        // should be redundant
        // add all value;
    } if (keyword == "limit_except") {
        // add all value;
    } if (keyword == "index") {
        // add all value;
    } if (keyword == "return") {
        // add all value;
    }
}

void parsing(const std::vector<TOKEN>& tokenList) {
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
                            i = locationPrint(tokenList, i);
                        } else {
                            std::cout << tokenList[i].content << ": ";
                            i++;
                            for (; i < tokenList.size(); ++i) {
                               if (tokenList[i].type == SEMICOLON) {
                                   std::cout << std::endl;
                                   break;
                               } if (tokenList[i].type == PARAM) {
                                   std::cout << tokenList[i].content << " ";
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
}
