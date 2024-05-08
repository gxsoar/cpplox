#pragma once

#include <error.h>
#include "ast.h"
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include "stmt.h"
#include "token.h"

namespace cpplox {

class Parser {
public:
  explicit Parser(const std::vector<Token> &token) : tokens_(token){}

  // auto Parse() -> std::shared_ptr<ExprAST>;
  auto Parse() -> std::vector<std::shared_ptr<Stmt>>;

private:
  auto Expression() -> std::shared_ptr<ExprAST> { return Assignment(); }
  auto Equality() -> std::shared_ptr<ExprAST>;
  auto Term() -> std::shared_ptr<ExprAST>;
  auto Comparsion() -> std::shared_ptr<ExprAST>;
  auto Factor() -> std::shared_ptr<ExprAST>;
  auto Unary() -> std::shared_ptr<ExprAST>;
  auto Primary() -> std::shared_ptr<ExprAST>;
  auto Assignment() -> std::shared_ptr<ExprAST>;
  auto Call() -> std::shared_ptr<ExprAST>;
  auto FinishCall(const std::shared_ptr<ExprAST> &callee) -> std::shared_ptr<ExprAST>;

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
  auto Statement() -> std::shared_ptr<Stmt>;
  auto IfStatement() -> std::shared_ptr<Stmt>; // 处理if语句
  auto PrintStatement() -> std::shared_ptr<Stmt>;
  auto WhileStatement() -> std::shared_ptr<Stmt>;
  auto And() -> std::shared_ptr<ExprAST>;
  auto Or() -> std::shared_ptr<ExprAST>; 
  auto ForStatement() -> std::shared_ptr<Stmt>;
  auto VarDeclaration() -> std::shared_ptr<Stmt>;
  auto ExpressionStatement() -> std::shared_ptr<Stmt>;
  auto Declaration() -> std::shared_ptr<Stmt>;
  auto Block() -> std::vector<std::shared_ptr<Stmt>>;
  auto Function(const std::string &kind) -> std::shared_ptr<FunctionStmt>;
  auto ReturnStatement() -> std::shared_ptr<Stmt>;
private:
  std::vector<Token> tokens_;
  int current_{0};
};

}  // namespace cpplox