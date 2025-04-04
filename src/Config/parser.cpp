#include "../../Includes/Config/parser.hpp"

#include <unordered_set>

void validateLocationParams(route& current_route, const std::string& keyword, const std::vector<std::string>& params) {
    if (keyword == "index") {
        if (current_route.getRouteIsRedirect() == true) {
            throw std::runtime_error(" index: mixing index and return is not allowed");
        }
        if (params.size() != 1) {
            throw std::runtime_error(" index: invalid arguments");
        }
        current_route.setIndex(params[0]);
    } else if (keyword == "allowed_methods") {
        if (current_route.getRouteIsRedirect() == true) {
            throw std::runtime_error(" index: mixing allowed_methods and return is not allowed");
        }
        if (params.empty()) {
            throw std::runtime_error(" allowed_methods: invalid arguments");
        }
        for (const auto & param : params) {
           if (param == "POST" || param == "GET" || param == "DELETE") {
               current_route.setAllowedMethods(param);
           } else {
               throw std::runtime_error(" allowed_methods: invalid arguments");
           }
        }
    } else if (keyword == "return") {
        // should be validated first
        if (!current_route.getAllowedMethods().empty() || !current_route.getAlias().empty()
            || !current_route.getRoot().empty() || current_route.getAutoIndex() == true
            || !current_route.getCgi().empty() || !current_route.getIndex().empty()) {
            throw std::runtime_error(" return: can't be mixed up with other keywords");
        }
        if (params.size() != 2) {
            throw std::runtime_error(" return: invalid arguments");
        }
        current_route.setRouteIsRedirect(true);
        current_route.setRedirect(stoul(params[0]), params[1]);
    } else if (keyword == "autoindex") {
        if (current_route.getRouteIsRedirect() == true) {
            throw std::runtime_error(" index: mixing autoindex and return is not allowed");
        }
        if (params.size() != 1) {
            throw std::runtime_error(" autoindex: invalid arguments");
        } if (params[0] == "on") {
            current_route.setAutoIndex(true);
        } else if (params[0] == "off") {
            current_route.setAutoIndex(false);
        } else {
            throw std::runtime_error(" autoindex: invalid arguments");
        }
    } else if (keyword == "root") {
        if (current_route.getRouteIsRedirect() == true) {
            throw std::runtime_error(" index: mixing root and return is not allowed");
        }
        // should be validated first
        if (params.size() != 1) {
            throw std::runtime_error(" root: invalid arguments");
        }
        if (!current_route.getAlias().empty()) {
            throw std::runtime_error(" root: mixing root and alias not allowed");
        }
        current_route.setRoot(params[0]);
    } else if (keyword == "cgi") {
        if (current_route.getRouteIsRedirect() == true) {
            throw std::runtime_error(" index: mixing cgi and return is not allowed");
        }
        // should be validated first
        if (params.size() != 2) {
            throw std::runtime_error(" cgi: invalid arguments");
        }
        current_route.setCgi(params[0], params[1]);
    } else if (keyword == "alias") {
        if (current_route.getRouteIsRedirect() == true) {
            throw std::runtime_error(" index: mixing alias and return is not allowed");
        }
        if (params.size() != 1) {
            throw std::runtime_error(" alias: invalid arguments");
        }
        if (!current_route.getRoot().empty()) {
            throw std::runtime_error(" alias: mixing root and alias not allowed");
        }
        // if (params[0].back() == '/') {
        //     throw std::runtime_error(" alias: no trailing slash allowed");
        // }
        current_route.setAlias(params[0]);
    } else {
        throw std::runtime_error(" " + keyword + " not allowed in location block");
    }
}

std::size_t locationPrint(serverConfig& server_config, const std::vector<TOKEN>& tokenList, std::size_t i) {
    if (i < tokenList.size() && tokenList[i].type == PARAM ) {
        if (tokenList[i].content.front() != '/') {
            throw std::runtime_error("line: " + std::to_string(tokenList[i].line) + " " + tokenList[i].content + " is not a valid URI pattern for a location");
        }
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
    } else if (keyword == "server_name") {
        for (const auto& param : params) {
            if (isValidServerName(param)) {
                server_config.setServerName(param);
            } else {
                throw std::runtime_error(" server_name: invalid arguments");
            }
        }
    } else if (keyword == "client_max_body_size") {
        if (params.size() != 1) {
            throw std::runtime_error(" client_max_body_size: invalid arguments");
        }
        std::size_t result;
        if (!isValidDataSize(params[0], result)) {
            throw std::runtime_error(" client_max_body_size: invalid arguments");
        }
        server_config.setClientMaxBodySize(result);
    } else if (keyword == "root") {
        if (params.size() != 1) {
            throw std::runtime_error(" root: invalid arguments");
        }
        // should be validated first
        server_config.setRoot(params[0]);
    } else if (keyword == "error_page") {
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
    } else if (keyword == "timeout") {
         if (params.size() != 1) {
             throw std::runtime_error(" timeout: invalid arguments");
         }
         double result;
         if (!isValidTime(params[0], result)) {
             throw std::runtime_error(" timeout: invalid arguments");
         }
         server_config.setTimeout(result);
    } else if (keyword == "index") {
        if (params.size() != 1) {
            throw std::runtime_error(" index: invalid arguments");
        }
        server_config.setIndex(params[0]);
    } else {
        throw std::runtime_error(" " + keyword + " not allowed in server block");
    }
}

bool    hasDuplicate(const std::vector<size_t>& vec) {
    std::unordered_set<size_t> seen;
    for (const auto& num : vec) {
        if (seen.count(num)) {
            return true;
        }
        seen.insert(num);
    }
    return false;
}

void update_routes(std::vector<serverConfig>& server_configs) {
   std::vector<size_t> ports;
   for (auto& server_config : server_configs) {
       ports.emplace_back(server_config.getPort());
       auto locations = server_config.getLocation();
       for (auto& [uri, Route] : locations) {
           route& temp = server_config.getRoute(uri);
           std::string raw_actual_path;
           if (temp.getRouteIsRedirect() == true) continue;
           // route without root and alias
           if (server_config.getRoute(uri).getRoot().empty() && server_config.getRoute(uri).getAlias().empty()) {
               std::string serverRoot = server_config.getRoot();
               if (!serverRoot.empty() && serverRoot.back() == '/') {
                   serverRoot.pop_back();
               }
               temp.setRoot(serverRoot);
               raw_actual_path = serverRoot + uri;
               // temp.setActualPath(serverRoot + uri);
               // route with alias
           } else if (!server_config.getRoute(uri).getAlias().empty()) {
               std::string aliasPath = server_config.getRoute(uri).getAlias();
               if (aliasPath.back() == '/') {
                   aliasPath.pop_back();
               }
               // if (aliasPath.front() != '/') {
               //     raw_actual_path = '/' + aliasPath;
               // }
               else {
                   raw_actual_path = aliasPath;
               }
               // temp.setActualPath(aliasPath);
               // route with root
           } else if(!server_config.getRoute(uri).getRoot().empty()) {
               std::string routeRoot = server_config.getRoute(uri).getRoot();
               if (routeRoot.back() == '/') {
                   routeRoot.pop_back();
               }
               raw_actual_path = routeRoot + uri;
               // temp.setActualPath(routeRoot + uri);
           }
           std::filesystem::path raw_path = raw_actual_path;
           if (raw_actual_path.back() != '/') {
               if (!is_regular_file(raw_path)) {
                   // std::cout << raw_actual_path << " is dir" << std::endl;
                   raw_actual_path.push_back('/');
               }
               // if (!std::filesystem::is_regular_file(raw_actual_path)) {
               //     raw_actual_path.push_back('/');
               // }
           }
           std::string currPath = std::filesystem::current_path();
           // std::cout << "currPath: " << currPath << std::endl;
           if (temp.getRoot().empty() && temp.getAlias().empty())  {
               temp.setActualPath(currPath  + raw_actual_path);
           } else if (is_regular_file(raw_path)) {
               temp.setActualPath(currPath + "/" + raw_actual_path);
           }
           else if (raw_path.is_absolute()) {
               temp.setActualPath(raw_actual_path);
           } else if (raw_path.is_relative()) {
               temp.setActualPath(currPath + "/" + raw_actual_path);

           }
           // std::cout << "raw actual path: " << raw_actual_path << std::endl;
           // std::cout << "ACTUAL PATH: " << temp.getActualPath() << std::endl;
           // temp.setActualPath(raw_actual_path);
           //  route without index - try to get index from server block
           if (server_config.getRoute(uri).getIndex().empty()) {
              temp.setIndex(server_config.getIndex());
           }
       }
   }
   if (hasDuplicate(ports)) {
       throw std::runtime_error("Detected the same Port in multiple servers");
   }
}

void addDefaultErrorPages(std::vector<serverConfig>& server_configs) {
    std::string errDir = "errorPages/";
    if (errDir.back() != '/') {
        errDir.push_back('/');
    }
    std::filesystem::path dir{errDir};
    if (!std::filesystem::is_directory(dir)) {
        std::cout << "Cannot load default error pages" << std::endl;
        return ;
    }
    for (auto& config : server_configs) {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string fileName = entry.path().filename().string();
                std::regex pattern(R"((\d{3})\.html)");
                std::smatch match;
                if (std::regex_match(fileName, match, pattern)) {
                    std::string numberStr = match[1].str();
                    std::size_t number = std::stoul(numberStr);
                    std::string path = errDir + fileName;
                    // std::cout << number << ": " << path << std::endl;
                    config.setErrorPages(number, path);
                } else {
                    std::cout <<  "An Error occurred during loading the default error pages" << std::endl;
                }
            }
        }
    }
}

void printServerConfig(std::vector<serverConfig>& servers) {
    for (auto& instance : servers) {
        instance.printData();
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
	printServerConfig(server_configs);
    addDefaultErrorPages(server_configs);
	// printServerConfig(server_configs);
    return server_configs;
}
