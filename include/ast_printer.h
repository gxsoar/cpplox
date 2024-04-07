#pragma once

#include <any>
#include <cassert>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include "ast.h"

namespace cpplox {

class ASTPrinter : public ExprASTVisitor{
public:
  auto Print(const std::shared_ptr<ExprAST>& expr_ast) -> std::string {
    return std::any_cast<std::string>(expr_ast->Accept(*this));
  }
  auto VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any override;
  auto VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr_ast) -> std::any override;
  auto VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr_ast) -> std::any override;
  auto VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any override;
private:
  template<typename... T>
  auto Parenthesize(const std::string &name, T... expr_ast) -> std::string {
    assert((... && std::is_same_v<T, std::shared_ptr<ExprAST>>));
    std::ostringstream builder;
    builder << "(" << name;
    ((builder << " " << Print(expr_ast)), ...);
    builder << ")";
    return builder.str();
  }
};

} // namespace cpplox