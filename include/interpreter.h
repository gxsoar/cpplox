#pragma once

#include <any>
#include <memory>
#include <string>
#include "ast.h"
#include "token.h"

namespace cpplox {

class Interpreter : public ExprASTVisitor {
public:
  auto VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr_ast) -> std::any override {
    return expr_ast->GetValue();
  }
  auto VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr_ast) -> std::any override {
    return Evaluate(expr_ast->GetExpression());
  }
  auto VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any override;
  auto VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any override;
  void Interpret(const std::shared_ptr<ExprAST>& expression);
  
private:
  auto Evaluate(const std::shared_ptr<ExprAST>& expression) -> std::any {
    return expression->Accept(*this);
  }
  auto IsTruthy(const std::any &value) -> bool;
  auto IsEqual(const std::any &left, const std::any &right) -> bool;
  void CheckNumberOperand(const Token &op, const std::any &left, const std::any &right);
  auto StringIfy(const std::any &value) -> std::string;
};

} // namespace cpplox