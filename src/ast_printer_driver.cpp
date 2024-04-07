#include <memory>
#include "ast.h"
#include "ast_printer.h"
#include "token.h"

auto main() -> int {
  auto expression = std::make_shared<cpplox::BinaryExprAST>(
      std::make_shared<cpplox::UnaryExprAST>(std::make_shared<cpplox::LiteralExprAST>(123),
                                             cpplox::Token(cpplox::TokenType::MINUS, "-", nullptr, 1)),
      cpplox::Token(cpplox::TokenType::STAR, "*", nullptr, 1),
      std::make_shared<cpplox::GroupingExprAST>(std::make_shared<cpplox::LiteralExprAST>(45.67)));
  auto ast_printer = std::make_unique<cpplox::ASTPrinter>();
  ast_printer->Print(expression);
  return 0;
}