#pragma once

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ast.h"
#include "interpreter.h"
#include "stmt.h"
#include "token.h"
namespace cpplox {

enum class FunctionType {
    NONE,
    FUNCTION,
    INITIALIZER,
    METHOD
};

enum class ClassType {
  NONE,
  CLASS,
  SUBCLASS
};

class Resolver : public ExprASTVisitor, StmtVisitor {
public:
  explicit Resolver(const std::shared_ptr<Interpreter> &interpreter) : interpreter_(interpreter) {}

  void VisitBlockStmt(std::shared_ptr<BlockStmt> stmt) override;
  void VisitVarStmt(std::shared_ptr<VarStmt> stmt) override;
  void VisitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) override;
  void VisitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) override;
  void VisitIfStmt(std::shared_ptr<IfStmt> stmt) override;
  void VisitPrintStmt(std::shared_ptr<PrintStmt> stmt) override;
  void VisitReturnStmt(std::shared_ptr<ReturnStmt> stmt) override;
  void VisitWhileStmt(std::shared_ptr<WhileStmt> stmt) override;
  void VisitClassStmt(std::shared_ptr<ClassStmt> stmt) override;

  auto VisitVariableExprAST(std::shared_ptr<VarExprAST> expr_ast) -> std::any override;
  auto VisitAssignmentExprAST(std::shared_ptr<AssignExprAST> expr_ast) -> std::any override;
  auto VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any override;
  auto VisitCallExprAST(std::shared_ptr<CallExprAST> expr_ast) -> std::any override;
  auto VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr_ast) -> std::any override;
  auto VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr) -> std::any override;
  auto VisitLogicalExprAST(std::shared_ptr<LogicalExprAST> expr_ast) -> std::any override;
  auto VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any override;
  auto VisitGetExprAST(std::shared_ptr<GetExprAST> expr_ast) -> std::any override;
  auto VisitSetExprAST(std::shared_ptr<SetExprAST> expr_ast) -> std::any override;
  auto VisitThisExprAST(std::shared_ptr<ThisExprAST> expr_ast) -> std::any override;
  auto VisitSuperExprAST(std::shared_ptr<SuperExprAST> expr_ast) -> std::any override;

  void Resolve(const std::vector<std::shared_ptr<Stmt>> &statements);
private:
  void BeginScope();
  void EndScope();
  
  void Resolve(const std::shared_ptr<Stmt> &statements);
  void Resolve(const std::shared_ptr<ExprAST> &expr);
  void Declare(const Token &name);
  void Define(const Token &name);
  void ResolveLocal(const std::shared_ptr<ExprAST> &expr, const Token &name);
  void ResolveFunction(const std::shared_ptr<FunctionStmt> &function, const FunctionType &function_type);
private:
  std::shared_ptr<Interpreter> interpreter_;
  std::vector<std::unordered_map<std::string, bool>> scopes_;
  FunctionType current_function_ {FunctionType::NONE};
  ClassType current_class_ {ClassType::NONE};
};

} // namespace cpplox