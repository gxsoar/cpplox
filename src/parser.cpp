#include <algorithm>
#include <initializer_list>
#include <memory>
#include <string>

#include "parser.h"
#include "ast.h"
#include "token.h"
#include "error.h"

namespace cpplox {

// 语法的优先级规则，优先级由低到高排序
// Equality == !=
// Comparison > >= < <=
// Term - +
// Factor / *
// Unary ! -

auto Parser::Parse() -> std::shared_ptr<ExprAST> {
  try {
    return Expression();
  } catch (ParseError error) {
    return nullptr;
  }
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

} // namespace cpplox