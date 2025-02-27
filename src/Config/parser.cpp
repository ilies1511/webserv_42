#include "../../Includes/Config/parser.hpp"

#include <serverConfig.hpp>


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
                        std::cout << "config file invalid" << std::endl;
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

void assignToObject(const std::vector<TOKEN>& tokenList, serverConfig& server_config, std::size_t i) {
    if (tokenList[i].content == "listen") {
        // add all value;
    } if (tokenList[i].content == "server_name") {
        // add all value;
    } if (tokenList[i].content == "client_max_body_size") {
        // add all value;
    } if (tokenList[i].content == "root") {
        // add all value;
    } if (tokenList[i].content == "error_page") {
        // add all value;
    } if (tokenList[i].content == "location") {
        // should be redundant
        // add all value;
    } if (tokenList[i].content == "limit_except") {
        // add all value;
    } if (tokenList[i].content == "index") {
        // add all value;
    } if (tokenList[i].content == "return") {
        // add all value;
    }
}

void parsing(const std::vector<TOKEN>& tokenList) {
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
                    if (tokenList[i].type == KEYWORD) {
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
