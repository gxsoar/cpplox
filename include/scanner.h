#pragma once
#include <string>
#include <utility>
#include <list>

#include "token.h"

namespace cpplox {

class Scanner {
public:
  explicit Scanner(std::string source) : source_(std::move(source)) {}
  auto ScanTokens() -> std::list<Token>;

private:
  auto IsAtEnd() -> bool;
  auto ScanToken() -> void;

private:
  std::string source_;
  std::list<Token> tokens_;
};

}  // namespace cpplox