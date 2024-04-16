#include <algorithm>
#include <initializer_list>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "parser.h"
#include "ast.h"
#include "stmt.h"
#include "token.h"
#include "error.h"

namespace cpplox {

// 语法的优先级规则，优先级由低到高排序
// Equality == !=
// Comparison > >= < <=
// Term - +
// Factor / *
// Unary ! -

// auto Parser::Parse() -> std::shared_ptr<ExprAST> {
//   try {
//     return Expression();
//   } catch (ParseError error) {
//     return nullptr;
//   }
// }

auto Parser::Parse() -> std::vector<std::shared_ptr<Stmt>> {
  std::vector<std::shared_ptr<Stmt>> statements;
  while(!IsAtEnd()) {
    statements.push_back(Declaration());
  }
  return statements;
}

auto Parser::Equality() -> std::shared_ptr<ExprAST> {
  auto expr_ast{Comparsion()};
  while(Match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
    Token op = Previous();
    auto right{Comparsion()};
    // Todo(gaoxiang): Add BinaryExprAST ctor parameter
    expr_ast = std::make_shared<BinaryExprAST>(expr_ast, op, right);
  }
  return nullptr;
}

// comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
auto Parser::Comparsion() -> std::shared_ptr<ExprAST> {
  auto expr_ast{Term()};
  while(Match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
    Token op = Previous();
    auto right {Term()};
    // Todo(gaoxiang): Add BinaryExprAST ctor parameter
    expr_ast = std::make_shared<BinaryExprAST>(expr_ast, op, right);
  }
  return expr_ast;
}

auto Parser::Term() -> std::shared_ptr<ExprAST> {
  auto expr_ast {Factor()};
  // 先做加减法
  while(Match({TokenType::MINUS, TokenType::PLUS})) {
    Token op = Previous();
    auto right {Factor()};
    expr_ast = std::make_shared<BinaryExprAST>(expr_ast, op, right);
  }
  return expr_ast;
}

// factor         → factor ( "/" | "*" ) unary | unary ;

auto Parser::Factor() -> std::shared_ptr<ExprAST> {
  auto expr_ast {Unary()};
  while(Match({TokenType::SLASH, TokenType::STAR})) {
    Token op {Previous()};
    auto right {Unary()};
    expr_ast = std::make_shared<BinaryExprAST>(expr_ast, op, right);
  }
  return expr_ast;
}

auto Parser::Match(const std::initializer_list<TokenType> &types) -> bool {
  return std::ranges::all_of(types.begin(), types.end(), [this](const TokenType &type) {
    if (Check(type)) {
      Advance();
      return true;
    }
    return false;
  });
}

// unary          → ( "!" | "-" ) unary | primary ;
// 处理一元运算符
auto Parser::Unary() -> std::shared_ptr<ExprAST> {
  if (Match({TokenType::BANG, TokenType::MINUS})) {
    Token op{Previous()};
    // 采用递归的方式来解析操作数
    auto right {Unary()};
    return std::make_shared<UnaryExprAST>(op, right);
  }
  return Primary();
}

// primary        → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
// 处理最高优先级
auto Parser::Primary() -> std::shared_ptr<ExprAST> {
  if (Match({TokenType::FALSE})) {
    return std::make_shared<LiteralExprAST>();
  }
  if (Match({TokenType::TRUE})) {
    return std::make_shared<LiteralExprAST>();
  }
  if (Match({TokenType::NIL})) {
    return std::make_shared<LiteralExprAST>();
  }

  if (Match({TokenType::NUMBER, TokenType::STRING})) {
    return std::make_shared<LiteralExprAST>();
  }

  if (Match({TokenType::IDENTIFIER})) {
    return std::make_shared<VarExprAST>(Previous());
  }
  if (Match({TokenType::LEFT_PAREN})) {
    auto expr_ast {Expression()};
    Consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
    return std::make_shared<GroupingExprAST>(expr_ast);
  }

  throw Error(Peek(), "Expect expression");
  return nullptr;
}

auto Parser::Consume(const TokenType &type, const std::string &message) -> Token {
  if (Check(type)) {
    return Advance();
  }
  throw Error(Peek(), message);
}

auto Parser::Check(const TokenType &type) -> bool {
  if (IsAtEnd()) {
    return false;
  }
  return Peek().GetTokenType() == type;
}

auto Parser::IsAtEnd() -> bool {
  return Peek().GetTokenType() == TokenType::TOKEN_EOF;
}

auto Parser::Peek() -> Token {
  return tokens_[current_];
}

auto Parser::Previous() -> Token {
  return tokens_[current_ - 1];
}

auto Parser::Advance() -> Token {
  if (!IsAtEnd()) {
    current_++;
  }
  return Previous();
}

void Parser::Synchronize() {
  Advance();
  while(!IsAtEnd()) {
    if (Previous().GetTokenType() == TokenType::SEMICOLON) {
      return;
    }
    switch (Peek().GetTokenType()) {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN:
        return;
      default:
        break;
    }
    Advance();
  }
}

auto Parser::IfStatement() -> std::shared_ptr<Stmt> {
  // deal with (
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  // if condition expression
  auto condition {Expression()};
  // deal with ')'
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");

  auto then_branch {Statement()};
  std::shared_ptr<Stmt> else_branch = nullptr;
  if (Match({TokenType::ELSE})) {
    else_branch = Statement();
  }

  return std::make_shared<IfStmt>(condition, then_branch, else_branch);
}

auto Parser::Statement() -> std::shared_ptr<Stmt> {
  if (Match({TokenType::FOR})) {
    return ForStatement();
  }

  if (Match({TokenType::IF})) {
    return IfStatement();
  }
  if (Match({TokenType::PRINT})) {
    return PrintStatement();
  }
  // Todo(gaoxiang)

  if (Match({TokenType::LEFT_BRACE})) {
    return std::make_shared<BlockStmt>(Block());
  }
  if (Match({TokenType::WHILE})) {
    return WhileStatement();
  }

  return ExpressionStatement();
}

auto Parser::PrintStatement() -> std::shared_ptr<Stmt> {
  auto value {Expression()};
  Consume(TokenType::SEMICOLON, "Expect ';' after value");
  return std::make_shared<Stmt>(value);
}

auto Parser::ExpressionStatement() -> std::shared_ptr<Stmt> {
  auto expr {Expression()};
  Consume(TokenType::SEMICOLON, "Expect ';' after expression");
  return std::make_shared<Stmt>(expr);
}

auto Parser::Or() -> std::shared_ptr<ExprAST> {
  auto expr {And()};
  while(Match({TokenType::OR})) {
    auto op {Previous()};
    auto right {And()};
    expr = std::make_shared<LogicalExprAST>(expr, op, right);
  }
  return expr;
}

auto Parser::And() -> std::shared_ptr<ExprAST> {
  auto expr {Equality()};
  while(Match({TokenType::AND})) {
    auto op {Previous()};
    auto right {Equality()};
    expr = std::make_shared<LogicalExprAST>(expr, op, right);
  }
  return expr;
}

auto Parser::WhileStatement() -> std::shared_ptr<Stmt> {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  auto condition {Expression()};
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  auto body {Statement()};
  return std::make_shared<WhileStmt>(condition, body);
}

auto Parser::ForStatement() -> std::shared_ptr<Stmt> {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'for' .");
  std::shared_ptr<Stmt> initializer;
  if (Match({TokenType::SEMICOLON})) {
    initializer = nullptr;
  } else if (Match({TokenType::VAR})) {
    initializer = VarDeclaration();
  } else {
    initializer = ExpressionStatement();
  }
  std::shared_ptr<ExprAST> condition;
  if (!Check(TokenType::SEMICOLON)) {
    condition = Expression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
  std::shared_ptr<ExprAST> increment;
  if (!Check(TokenType::RIGHT_PAREN)) {
    increment = Expression();
  }
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses");
  auto body {Statement()};

  if (increment != nullptr) {
    std::vector<std::shared_ptr<Stmt>> statements {std::make_shared<ExpressionStmt>(increment)};
    body = std::make_shared<BlockStmt>(statements);
  }

  if (condition == nullptr) {
    condition = std::make_shared<LiteralExprAST>(true);
    body = std::make_shared<WhileStmt>(condition, body);
  }

  if (initializer != nullptr) {
    std::vector<std::shared_ptr<Stmt>> statements {std::make_shared<ExpressionStmt>(initializer)};
    body = std::make_shared<BlockStmt>(statements);
  }
  return body;
}

auto Parser::Declaration() -> std::shared_ptr<Stmt> {
  try {
  if (Match({TokenType::VAR})) {
    return VarDeclaration();
  }
  } catch (ParseError error) {
    Synchronize();
    return nullptr;
  }
  return nullptr;
}

auto Parser::VarDeclaration() -> std::shared_ptr<Stmt> {
  auto name {Consume(TokenType::IDENTIFIER, "Expect variable name.")};
  std::shared_ptr<ExprAST> initializer;
  if (Match({TokenType::EQUAL})) {
    initializer = Expression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");
  return std::make_shared<VarStmt>(name, initializer);
}

auto Parser::Assignment() -> std::shared_ptr<ExprAST> {
  auto expr {Equality()};

  if (Match({TokenType::EQUAL})) {
    auto equals {Previous()};
    auto value {Assignment()};
    if (auto *e = dynamic_cast<VarExprAST*>(value.get())) {
      auto name {e->GetToken()};
      return std::make_shared<AssignExprAST>(name, value);
    }

    Log::Error(equals, "Invalid assignment target.");
  }

  return expr;
}

auto Parser::Block() -> std::vector<std::shared_ptr<Stmt>> {
  std::vector<std::shared_ptr<Stmt>> statements;
  while(!Check({TokenType::RIGHT_BRACE}) && !IsAtEnd()) {
    statements.push_back(Declaration());
  }
  Consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
  return statements;
}

} // namespace cpplox