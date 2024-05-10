#pragma once

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
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
    // return environment_->Get(expr_ast->GetToken());
    return LookUpVariable(expr_ast->GetToken(),expr_ast);
  }
  auto VisitLogicalExprAST(std::shared_ptr<LogicalExprAST> expr_ast) -> std::any override;
  auto VisitAssignmentExprAST(std::shared_ptr<AssignExprAST> expr_ast) -> std::any override;
  auto VisitCallExprAST(std::shared_ptr<CallExprAST> expr_ast) -> std::any override;
  auto VisitGetExprAST(std::shared_ptr<GetExprAST> expr_ast) -> std::any override;
  auto VisitSetExprAST(std::shared_ptr<SetExprAST> expr_ast) -> std::any override;
  auto VisitThisExprAST(std::shared_ptr<ThisExprAST> expr_ast) -> std::any override;

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
  void VisitClassStmt(std::shared_ptr<ClassStmt> stmt) override;

  void ExecuteBlock(const std::vector<std::shared_ptr<Stmt>> &statements, const std::shared_ptr<Environment> &env);
  auto GetGlobalEnvironment() const -> std::shared_ptr<Environment> { return globals_; }
  void Resolve(std::shared_ptr<ExprAST> expr, std::unordered_map<std::string, bool> &scope);
  void Resolve(const std::shared_ptr<ExprAST> &expr, int depth);

private:
  auto Evaluate(const std::shared_ptr<ExprAST>& expression) -> std::any {
    return expression->Accept(*this);
  }
  auto IsTruthy(const std::any &value) -> bool;
  auto IsEqual(const std::any &left, const std::any &right) -> bool;
  void CheckNumberOperand(const Token &op, const std::any &left, const std::any &right);
  auto StringIfy(const std::any &value) -> std::string;
  void Execute(const std::shared_ptr<Stmt> &stmt);
  auto LookUpVariable(const Token &name, const std::shared_ptr<ExprAST> &expr) -> std::any;

private:
  std::shared_ptr<Environment> globals_{std::make_shared<Environment>()};
  std::shared_ptr<Environment> environment_{globals_};
  std::unordered_map<std::shared_ptr<ExprAST>, int> locals_;
};

} // namespace cpplox