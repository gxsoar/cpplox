#pragma once

#include <error.h>
#include "ast.h"
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "token.h"

namespace cpplox {

class Parser {
public:
  explicit Parser(const std::vector<Token> &token) : tokens_(token){}

  auto Parse() -> std::shared_ptr<ExprAST>;

private:
  auto Expression() -> std::shared_ptr<ExprAST> { return Equality(); }
  auto Equality() -> std::shared_ptr<ExprAST>;
  auto Term() -> std::shared_ptr<ExprAST>;
  auto Comparsion() -> std::shared_ptr<ExprAST>;
  auto Factor() -> std::shared_ptr<ExprAST>;
  auto Unary() -> std::shared_ptr<ExprAST>;
  auto Primary() -> std::shared_ptr<ExprAST>;

  auto Consume(const TokenType& type, const std::string &message) -> Token;

  // auto Match(const TokenType &lhs, const TokenType &rhs) -> bool;
  auto Match(const std::initializer_list<TokenType> &types) -> bool;
  auto Previous() -> Token;
  auto IsAtEnd() -> bool;
  auto Peek() -> Token;
  auto Advance() -> Token;
  auto Check(const TokenType& type) -> bool;

  void Synchronize();

private:
  struct ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
    
  };
  auto Error(const Token &token, const std::string &message) -> ParseError {
    Log::Error(token, message);
    return ParseError{""};
  }
private:
  std::vector<Token> tokens_;
  int current_{0};
};

}  // namespace cpplox