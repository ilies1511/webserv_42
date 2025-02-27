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

struct TOKEN {
  TYPE         type;
  std::string  content;

  friend std::ostream& operator<<(std::ostream& os, const TOKEN& t);
};

void        getToken(const std::string& input, std::vector<TOKEN>& tokenList);
void        tokenizer(const std::string& word, std::vector<TOKEN>& tokenList);
bool        check_keywords(const std::string& keyword);
std::string typeToString(TYPE type);
void        printToken(std::vector<TOKEN> tokenList);