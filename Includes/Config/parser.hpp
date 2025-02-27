#pragma once
#include "../../Includes/Config/token.hpp"
#include <serverConfig.hpp>
#include <validation.hpp>

std::size_t locationPrint(const std::vector<TOKEN>& tokenList, std::size_t i);
void        validateParam(serverConfig& server_config, const std::string& keyword, const std::string& param);
void        parsing(const std::vector<TOKEN>& tokenList);

