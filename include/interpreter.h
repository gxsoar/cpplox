#pragma once

#include <any>
#include <memory>
#include <string>
#include <vector>
#include "ast.h"
#include "environment.h"
#include "stmt.h"
#include "token.h"

namespace cpplox {

class Interpreter : public ExprASTVisitor, public StmtVisitor {
public:
  auto VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr_ast) -> std::any override {
    return expr_ast->GetValue();
  }
  auto VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr_ast) -> std::any override {
    return Evaluate(expr_ast->GetExpression());
  }
  auto VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any override;
  auto VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any override;
  auto VisitVariableExprAST(std::shared_ptr<VarExprAST> expr_ast) -> std::any override {
    return environment_->Get(expr_ast->GetToken());
  }
  auto VisitLogicalExprAST(std::shared_ptr<LogicalExprAST> expr_ast) -> std::any override;
  auto VisitAssignmentExprAST(std::shared_ptr<AssignExprAST> expr_ast) -> std::any override;
  auto VisitCallExprAST(std::shared_ptr<CallExprAST> expr_ast) -> std::any override;

  void Interpret(const std::shared_ptr<ExprAST>& expression);
  void Interpret(const std::vector<std::shared_ptr<Stmt>> &statements);
  
  void VisitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) override;
  void VisitIfStmt(std::shared_ptr<IfStmt> stmt) override;
  void VisitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;
  void VisitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
  void VisitVarStmt(std::shared_ptr<VarStmt> stmt) override;
  void VisitBlockStmt(std::shared_ptr<BlockStmt> stmt) override {
    ExecuteBlock(stmt->GetBlockStatements(), environment_);
  }
  void VisitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) override;
  void VisitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
  void ExecuteBlock(const std::vector<std::shared_ptr<Stmt>> &statements, const std::shared_ptr<Environment> &env);
  auto GetGlobalEnvironment() const -> std::shared_ptr<Environment> { return globals_; }

private:
  auto Evaluate(const std::shared_ptr<ExprAST>& expression) -> std::any {
    return expression->Accept(*this);
  }
  auto IsTruthy(const std::any &value) -> bool;
  auto IsEqual(const std::any &left, const std::any &right) -> bool;
  void CheckNumberOperand(const Token &op, const std::any &left, const std::any &right);
  auto StringIfy(const std::any &value) -> std::string;
  void Execute(const std::shared_ptr<Stmt> &stmt);

private:
  std::shared_ptr<Environment> globals_{std::make_shared<Environment>()};
  std::shared_ptr<Environment> environment_{globals_};
};

} // namespace cpplox