#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

enum TYPE {
  KEYWORD,
  PARAM,
  LBRACE,
  RBRACE,
  SEMICOLON
};

enum KEYWORDS {
  HTTP,
  SERVER,
  LISTEN,
  SERVER_NAME,
  CLIENT_MAX_BODY_SIZE,
  ROOT,
  ERROR_PAGE,
  LOCATION,
  LIMIT_EXCEPT,
  INDEX,
  RETURN,
  TIMEOUT
};

struct TOKEN {
  TYPE         type;
  std::string  content;

  friend std::ostream& operator<<(std::ostream& os, const TOKEN& t);
};

void        getToken(const std::string& input, std::vector<TOKEN>& tokenList);
void        tokenizer(const std::string& word, std::vector<TOKEN>& tokenList);
bool        check_keywords(const std::string& keyword);
std::string typeToString(TYPE type);
void        printToken(const std::vector<TOKEN>& tokenList);