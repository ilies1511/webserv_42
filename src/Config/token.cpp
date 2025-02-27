#include "../../Includes/Config/token.hpp"
//#include "token.hpp"
#include <vector>
//#include "webserv.hpp"

void printToken(const std::vector<TOKEN>& tokenList) {
 	for (const auto& token : tokenList) {
		std::cout << token << std::endl;
	}
}

std::string typeToString(TYPE type) {
  if (type == KEYWORD) return "KEYWORD";
  if (type == PARAM) return "PARAM";
  if (type == LBRACE) return "LBRACE";
  if (type == RBRACE) return "RBRACE";
  if (type == SEMICOLON) return "SEMICOLON";
  return  "SOMETHINGWENTWRONG";
}

std::ostream& operator<<(std::ostream& os, const TOKEN& t) {
	os << "[" << typeToString(t.type) << ", " << t.content << "]";
    return os;
}

bool check_keywords(const std::string& keyword) {
	if (keyword == "server" ||
		keyword == "listen" ||
		keyword == "server_name" ||
		keyword == "client_max_body_size" ||
		keyword == "root" ||
		keyword == "error_page" ||
		keyword == "autoindex" ||
		keyword == "index" ||
		keyword == "return" ||
		keyword == "limit_except" ||
		keyword == "location") {
		return true;
		}
	return false;
}

void tokenizer(const std::string& word, std::vector<TOKEN>& tokenList) {

	if (word == ";") {
		tokenList.push_back({SEMICOLON, word});
	} else if (word == "{") {
		tokenList.push_back({LBRACE, word});
	} else  if (word == "}") {
		tokenList.push_back({RBRACE, word});
	} else if (check_keywords(word)) {
		tokenList.push_back({KEYWORD, word});
	} else if (word[0] == '#') {
        return ;
		// std::cout << "COMMENT";
	} else {
		tokenList.push_back({PARAM, word});
	}
}

void getToken(const std::string& input, std::vector<TOKEN>& tokenList) {
	std::ifstream file(input);
	if (!file) {
		std::cerr << "Cannot open file!" << std::endl;
		return ;
	}
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream stream(line);
		std::string word;
		while (stream >> word) {
			if (word.front() == '#') {
				break;
			}
			if (!word.empty() && word.back() == ';') {
				std::string baseWord = word.substr(0, word.size() - 1);
				if (word.length() > 1) {
					tokenizer(baseWord, tokenList);
				}
				tokenizer(";", tokenList);
			} else {
				tokenizer(word, tokenList);
			}
		}
	}
};