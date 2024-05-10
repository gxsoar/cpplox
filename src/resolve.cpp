#include <error.h>
#include <algorithm>
#include <any>
#include <memory>
#include <vector>
#include "ast.h"
#include "resolver.h"
#include "stmt.h"
#include "token.h"

namespace cpplox {

void Resolver::Resolve(const std::vector<std::shared_ptr<Stmt>> &statements) {
  for (auto &statement : statements) {
    Resolve(statement);
  }
}

void Resolver::Resolve(const std::shared_ptr<Stmt> &statement) {
  statement->Accept(*this);
}

void Resolver::Resolve(const std::shared_ptr<ExprAST> &expr) {
  expr->Accept(*this);
}

void Resolver::BeginScope() {
  // scopes_.emplace();
}

void Resolver::EndScope() {
  scopes_.pop_back();
}

void Resolver::VisitVarStmt(std::shared_ptr<VarStmt> stmt) {
  Declare(stmt->GetName());
  if (stmt->GetExpr() != nullptr) {
    Resolve(stmt->GetExpr());
  }
  Define(stmt->GetName());
}

void Resolver::Declare(const Token &name) {
  if (scopes_.empty()) {
    return;
  }
  auto &scope = scopes_.back();
  if (scope.contains(name.GetTokenLexeme())) {
    Log::Error(name, "Already variable with this name in this scope");
  }
  scope.emplace(name.GetTokenLexeme(), false);
}

void Resolver::Define(const Token &name) {
  if  (scopes_.empty()) {
    return;
  }
  scopes_.front().emplace(name.GetTokenLexeme(), true);
}

auto Resolver::VisitVariableExprAST(std::shared_ptr<VarExprAST> expr) -> std::any{
  if (!scopes_.empty() && !scopes_.front()[expr->GetToken().GetTokenLexeme()]) {
    Log::Error(expr->GetToken(), "Can`t read local variable in its own initializer.");
  }
  ResolveLocal(expr, expr->GetToken());
  return {};
}

void Resolver::ResolveLocal(const std::shared_ptr<ExprAST> &expr, const Token &name) {
  for (int i = scopes_.size() - 1; i >= 0; -- i) {
    if (scopes_[i].contains(name.GetTokenLexeme())) {
      interpreter_->Resolve(expr, scopes_[scopes_.size() - 1 - i]);
      return;
    }
  }
}

auto Resolver::VisitAssignmentExprAST(std::shared_ptr<AssignExprAST> expr) -> std::any {
  Resolve(expr->GetValue());
  ResolveLocal(expr, expr->GetName());
  return {};
}

void Resolver::VisitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) {
  Declare(stmt->GetFunctionName());
  Define(stmt->GetFunctionName());
  ResolveFunction(stmt, FunctionType::FUNCTION);
}

void Resolver::ResolveFunction(const std::shared_ptr<FunctionStmt> &function, const FunctionType &function_type) {
  FunctionType enclosing_function {current_function_};
  current_function_ = function_type;
  BeginScope();
  for (const auto &param : function->GetFunctionParams()) {
    Declare(param);
    Define(param);
  }
  Resolve(function->GetFunctionBody());
  current_function_ = enclosing_function;
  EndScope();
}

void Resolver::VisitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) {
  Resolve(stmt->GetExpr());
}

void Resolver::VisitIfStmt(std::shared_ptr<IfStmt> stmt) {
  Resolve(stmt->GetConditionExpression());
  Resolve(stmt->GetThenBranch());
  if (stmt->GetElseBranch() != nullptr) {
    Resolve(stmt->GetElseBranch());
  }
}

void Resolver::VisitPrintStmt(std::shared_ptr<PrintStmt> stmt) {
  Resolve(stmt->GetExpr());
}

void Resolver::VisitReturnStmt(std::shared_ptr<ReturnStmt> stmt) {
  if (current_function_ == FunctionType::NONE) {
    Log::Error(stmt->GetReturnKeyWord(), "Can`t return from top-level code.");
  }
  if (stmt->GetReturnValue() != nullptr) {
    if (current_function_ == FunctionType::INITIALIZER) {
      Log::Error(stmt->GetReturnKeyWord(), "Can`t return a value from an initializer.");
    }
    Resolve(stmt->GetReturnValue());
  }
}

void Resolver::VisitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
  Resolve(stmt->GetConditionExpr());
  Resolve(stmt->GetWhileBody());
}

auto Resolver::VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr) -> std::any {
  Resolve(expr->GetLeftExpr());
  Resolve(expr->GetRightExpr());
  return {};
}

auto Resolver::VisitCallExprAST(std::shared_ptr<CallExprAST> expr) -> std::any {
  Resolve(expr->GetCallee());
  for (const auto &argument : expr->GetArguments()) {
    Resolve(argument);
  }
  return {};
}

auto Resolver::VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr) -> std::any {
  Resolve(expr->GetExpression());
  return {};
}

auto Resolver::VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr) -> std::any { return {}; }

auto Resolver::VisitLogicalExprAST(std::shared_ptr<LogicalExprAST> expr_ast) -> std::any {
  Resolve(expr_ast->GetLeftExpr());
  Resolve(expr_ast->GetRightExpr());
  return {};
}

auto Resolver::VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any {
  Resolve(expr_ast->GetRightExpr());
  return {};
}

void Resolver::VisitClassStmt(std::shared_ptr<ClassStmt> stmt) {
  auto enclosing_class {current_class_};
  current_class_ = ClassType::CLASS;
  Declare(stmt->GetClassName());
  Define(stmt->GetClassName());
  BeginScope();
  scopes_.front().emplace("this", true);
  for (const auto &method : stmt->GetClassMethods()) {
    auto declaration {FunctionType::METHOD};
    if (method->GetFunctionName().GetTokenLexeme() == "init") {
      declaration = FunctionType::INITIALIZER;
    }
    ResolveFunction(method, declaration);
  }
  EndScope();
  current_class_ = enclosing_class;
}

auto Resolver::VisitGetExprAST(std::shared_ptr<GetExprAST> expr_ast) -> std::any {
  Resolve(expr_ast->GetObject());
  return {};
}

auto Resolver::VisitSetExprAST(std::shared_ptr<SetExprAST> expr_ast) -> std::any {
  Resolve(expr_ast->GetSetValue());
  Resolve(expr_ast->GetSetObject());
  return {};
}

auto Resolver::VisitThisExprAST(std::shared_ptr<ThisExprAST> expr_ast) -> std::any {
  if (current_class_ == ClassType::NONE) {
    Log::Error(expr_ast->GetThisKeyWord(), "Can`t use 'this' outside of a class");
    return {};
  }
  ResolveLocal(expr_ast, expr_ast->GetThisKeyWord());
  return {};
}

} // namespace cpplox