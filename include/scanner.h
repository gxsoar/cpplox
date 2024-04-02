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
  auto Advance() -> char;
  auto AddToken(TokenType token_type) -> void;
  auto AddToken(TokenType token_type, const std::string &val) -> void;
  auto Match(char expected) -> bool;
  auto Peek() -> char;
  auto String() -> void;
  auto Number() -> void;
  auto PeekNext() -> char;
  auto Identifier() -> void;
  auto IsAlphaNumeric(char ch) -> bool;

private:
  std::string source_;
  std::list<Token> tokens_;
  int start_{0};  // record begin pos
  int current_{0};  // record current pos
  int line_{1}; // record current source file line
};

}  // namespace cpplox