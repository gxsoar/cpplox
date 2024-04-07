#include "ast_printer.h"
#include <any>
#include <memory>
#include <string>
#include "ast.h"

namespace cpplox {

auto ASTPrinter::VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any {
  return Parenthesize(expr_ast->GetOperation().GetTokenLexeme(), 
          expr_ast->GetLeftExpr(), expr_ast->GetRightExpr());
}

auto ASTPrinter::VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr_ast) -> std::any {
  return Parenthesize("group", expr_ast->GetExpression());
}

auto ASTPrinter::VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr_ast) -> std::any {
  auto &value_type = expr_ast->GetValue().type();
  if (value_type == typeid(nullptr)) {
    return "nil";
  } if (value_type == typeid(std::string)) {
    return std::any_cast<std::string>(expr_ast->GetValue());
  } if (value_type == typeid(double)) {
    return std::any_cast<double>(expr_ast->GetValue());
  } if (value_type == typeid(bool)) {
    return std::any_cast<bool>(expr_ast->GetValue()) ? "true" : "false";
  }
  return "Error in VisitLiteralExprAST : literal type not recognized";
}

auto ASTPrinter::VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any {
  return Parenthesize(expr_ast->GetOperation().GetTokenLexeme(),expr_ast->GetRightExpr());
}

} // namespace cpplox