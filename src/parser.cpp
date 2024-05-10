#include <algorithm>
#include <initializer_list>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "ast.h"
#include "error.h"
#include "parser.h"
#include "stmt.h"
#include "token.h"

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
  while (!IsAtEnd()) {
    statements.push_back(Declaration());
  }
  return statements;
}

auto Parser::Equality() -> std::shared_ptr<ExprAST> {
  auto expr_ast{Comparsion()};
  while (Match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
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
  while (Match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
    Token op = Previous();
    auto right{Term()};
    // Todo(gaoxiang): Add BinaryExprAST ctor parameter
    expr_ast = std::make_shared<BinaryExprAST>(expr_ast, op, right);
  }
  return expr_ast;
}

auto Parser::Term() -> std::shared_ptr<ExprAST> {
  auto expr_ast{Factor()};
  // 先做加减法
  while (Match({TokenType::MINUS, TokenType::PLUS})) {
    Token op = Previous();
    auto right{Factor()};
    expr_ast = std::make_shared<BinaryExprAST>(expr_ast, op, right);
  }
  return expr_ast;
}

// factor         → factor ( "/" | "*" ) unary | unary ;

auto Parser::Factor() -> std::shared_ptr<ExprAST> {
  auto expr_ast{Unary()};
  while (Match({TokenType::SLASH, TokenType::STAR})) {
    Token op{Previous()};
    auto right{Unary()};
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
    auto right{Unary()};
    return std::make_shared<UnaryExprAST>(op, right);
  }
  return Call();
}

// primary        → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
// 处理最高优先级
auto Parser::Primary() -> std::shared_ptr<ExprAST> {
  if (Match({TokenType::FALSE})) {
    return std::make_shared<LiteralExprAST>(false);
  }
  if (Match({TokenType::TRUE})) {
    return std::make_shared<LiteralExprAST>(true);
  }
  if (Match({TokenType::NIL})) {
    return std::make_shared<LiteralExprAST>(nullptr);
  }

  if (Match({TokenType::NUMBER, TokenType::STRING})) {
    return std::make_shared<LiteralExprAST>(Previous().GetLiteral());
  }

  if (Match({TokenType::SUPER})) {
    auto keyword {Previous()};
    Consume(TokenType::DOT, "Expect '.' after super .");
    auto method {Consume(TokenType::IDENTIFIER, "Expect supper class method name")};
    return std::make_shared<SuperExprAST>(keyword, method);
  }

  if (Match({TokenType::THIS})) {
    return std::make_shared<ThisExprAST>(Previous());
  }

  if (Match({TokenType::IDENTIFIER})) {
    return std::make_shared<VarExprAST>(Previous());
  }
  if (Match({TokenType::LEFT_PAREN})) {
    auto expr_ast{Expression()};
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

auto Parser::IsAtEnd() -> bool { return Peek().GetTokenType() == TokenType::TOKEN_EOF; }

auto Parser::Peek() -> Token { return tokens_[current_]; }

auto Parser::Previous() -> Token { return tokens_[current_ - 1]; }

auto Parser::Advance() -> Token {
  if (!IsAtEnd()) {
    current_++;
  }
  return Previous();
}

void Parser::Synchronize() {
  Advance();
  while (!IsAtEnd()) {
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
  auto condition{Expression()};
  // deal with ')'
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition");

  auto then_branch{Statement()};
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
  if (Match({TokenType::RETURN})) {
    return ReturnStatement();
  }
  if (Match({TokenType::LEFT_BRACE})) {
    return std::make_shared<BlockStmt>(Block());
  }
  if (Match({TokenType::WHILE})) {
    return WhileStatement();
  }

  return ExpressionStatement();
}

auto Parser::PrintStatement() -> std::shared_ptr<Stmt> {
  auto value{Expression()};
  Consume(TokenType::SEMICOLON, "Expect ';' after value");
  return std::make_shared<Stmt>(value);
}

auto Parser::ExpressionStatement() -> std::shared_ptr<Stmt> {
  auto expr{Expression()};
  Consume(TokenType::SEMICOLON, "Expect ';' after expression");
  return std::make_shared<PrintStmt>(expr);
}

auto Parser::Or() -> std::shared_ptr<ExprAST> {
  auto expr{And()};
  while (Match({TokenType::OR})) {
    auto op{Previous()};
    auto right{And()};
    expr = std::make_shared<LogicalExprAST>(expr, op, right);
  }
  return expr;
}

auto Parser::And() -> std::shared_ptr<ExprAST> {
  auto expr{Equality()};
  while (Match({TokenType::AND})) {
    auto op{Previous()};
    auto right{Equality()};
    expr = std::make_shared<LogicalExprAST>(expr, op, right);
  }
  return expr;
}

auto Parser::WhileStatement() -> std::shared_ptr<Stmt> {
  Consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  auto condition{Expression()};
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  auto body{Statement()};
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
  auto body{Statement()};

  if (increment != nullptr) {
    std::vector<std::shared_ptr<Stmt>> statements{std::make_shared<ExpressionStmt>(increment)};
    body = std::make_shared<BlockStmt>(statements);
  }

  if (condition == nullptr) {
    condition = std::make_shared<LiteralExprAST>(true);
    body = std::make_shared<WhileStmt>(condition, body);
  }

  if (initializer != nullptr) {
    std::vector<std::shared_ptr<Stmt>> statements{std::make_shared<ExpressionStmt>(initializer)};
    body = std::make_shared<BlockStmt>(statements);
  }
  return body;
}

auto Parser::Declaration() -> std::shared_ptr<Stmt> {
  try {
    if (Match({TokenType::CLASS})) {
      return ClassDeclaration();
    }
    if (Match({TokenType::FUN})) {
      return Function("function");
    }
    if (Match({TokenType::VAR})) {
      return VarDeclaration();
    }
  } catch (ParseError error) {
    Synchronize();
    return nullptr;
  }
  return nullptr;
}

auto Parser::ClassDeclaration() -> std::shared_ptr<Stmt> {
  auto name {Consume(TokenType::IDENTIFIER, "Expect class name.")};
  std::shared_ptr<VarExprAST> supper_class;
  if (Match({TokenType::LESS})) {
    Consume(TokenType::IDENTIFIER, "Expect supper class name.");
    supper_class = std::make_shared<VarExprAST>(Previous());
  }
  Consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");
  // use std::vector<std::shared_ptr<Stmt>> ? or FunctionStmt
  std::vector<std::shared_ptr<Stmt>> methods;
  while(!Check({TokenType::RIGHT_BRACE}) && !IsAtEnd()) {
    methods.push_back(Function("method"));
  }
  Consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
  return std::make_shared<ClassStmt>(name, supper_class, methods);
}

auto Parser::VarDeclaration() -> std::shared_ptr<Stmt> {
  auto name{Consume(TokenType::IDENTIFIER, "Expect variable name.")};
  std::shared_ptr<ExprAST> initializer;
  if (Match({TokenType::EQUAL})) {
    initializer = Expression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after variable declaration");
  return std::make_shared<VarStmt>(name, initializer);
}

auto Parser::Assignment() -> std::shared_ptr<ExprAST> {
  auto expr{Equality()};

  if (Match({TokenType::EQUAL})) {
    auto equals{Previous()};
    auto value{Assignment()};
    if (auto *e = dynamic_cast<VarExprAST *>(value.get())) {
      auto name{e->GetToken()};
      return std::make_shared<AssignExprAST>(name, value);
    } 
    if (auto *e = dynamic_cast<GetExprAST*>(value.get())) {
      return std::make_shared<SetExprAST>(e->GetObject(), e->GetName(), value);
    }

    Log::Error(equals, "Invalid assignment target.");
  }

  return expr;
}

auto Parser::Block() -> std::vector<std::shared_ptr<Stmt>> {
  std::vector<std::shared_ptr<Stmt>> statements;
  while (!Check({TokenType::RIGHT_BRACE}) && !IsAtEnd()) {
    statements.push_back(Declaration());
  }
  Consume(TokenType::RIGHT_BRACE, "Expect '}' after block");
  return statements;
}

auto Parser::Call() -> std::shared_ptr<ExprAST> {
  auto expr = Primary();
  while (true) {
    if (Match({TokenType::LEFT_PAREN})) {
      expr = FinishCall(expr);
    } else if(Match({TokenType::DOT})){
      auto name {Consume(TokenType::IDENTIFIER, "Expect property name after '.' .")};
      expr = std::make_shared<GetExprAST>(expr, name);
    } else {
      break;
    }
  }
  return expr;
}

auto Parser::FinishCall(const std::shared_ptr<ExprAST> &callee) -> std::shared_ptr<ExprAST> {
  std::vector<std::shared_ptr<ExprAST>> arguments;
  if (!Check({TokenType::RIGHT_PAREN})) {
    do {
      if (arguments.size() >= 255) {
        Error(Peek(), "Can`t have more than 255 arguments");
      }
      arguments.emplace_back(Expression());
    } while (Match({TokenType::COMMA}));
  }
  auto paren{Consume({TokenType::RIGHT_PAREN}, "Expect ')' after arguments.")};
  return std::make_shared<CallExprAST>(callee, paren, arguments);
}

auto Parser::Function(const std::string &kind) -> std::shared_ptr<Stmt> {
  Token name{Consume(TokenType::IDENTIFIER, "Expect" + kind + " name.")};
  Consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
  std::vector<Token> parameters;
  if (!Check(TokenType::RIGHT_PAREN)) {
    do {
      if (parameters.size() >= 255) {
        Log::Error(Peek(), "Can`t have more than 255 parameters");
      }
      parameters.emplace_back(Consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (Match({TokenType::COMMA}));
  }
  Consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
  Consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
  auto body{Block()};
  return std::make_shared<FunctionStmt>(name, parameters, body);
}

auto Parser::ReturnStatement() -> std::shared_ptr<Stmt> {
  auto keyword{Previous()};
  std::shared_ptr<ExprAST> value;
  if (!Check({TokenType::SEMICOLON})) {
    value = Expression();
  }
  Consume(TokenType::SEMICOLON, "Expect ';' after return value.");
  return std::make_shared<ReturnStmt>(keyword, value);
}

}  // namespace cpplox