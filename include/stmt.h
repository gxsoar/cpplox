#pragma once

#include <memory>
#include <utility>
#include <vector>
#include "ast.h"
#include "token.h"
namespace cpplox {

class IfStmt;
class Stmt;
class WhileStmt;
class ExpressionStmt;
class PrintStmt;
class VarStmt;
class BlockStmt;
class FunctionStmt;
class ReturnStmt;

class StmtVisitor {
 public:
  virtual void VisitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) = 0;
  virtual void VisitIfStmt(std::shared_ptr<IfStmt> stmt) = 0;
  virtual void VisitWhileStmt(std::shared_ptr<WhileStmt> stmt) = 0;
  virtual void VisitPrintStmt(std::shared_ptr<PrintStmt> stmt) = 0;
  virtual void VisitVarStmt(std::shared_ptr<VarStmt> stmt) = 0;
  virtual void VisitBlockStmt(std::shared_ptr<BlockStmt> stmt) = 0;
  virtual void VisitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) = 0;
  virtual void VisitReturnStmt(std::shared_ptr<ReturnStmt> stmt) = 0;
};

class Stmt {
 public:
  virtual ~Stmt() = default;
  virtual void Accept(const StmtVisitor &) = 0;
};

class IfStmt : public Stmt, std::enable_shared_from_this<IfStmt> {
 public:
  IfStmt(std::shared_ptr<ExprAST> cond_expression, std::shared_ptr<Stmt> then_branch, std::shared_ptr<Stmt> else_branch)
      : cond_expression_(std::move(cond_expression)),
        then_branch_(std::move(then_branch)),
        else_branch_(std::move(else_branch)) {}
  auto GetConditionExpression() const -> std::shared_ptr<ExprAST> { return cond_expression_; }
  auto GetThenBranch() const -> std::shared_ptr<Stmt> { return then_branch_; }
  auto GetElseBranch() const -> std::shared_ptr<Stmt> { return else_branch_; }
 private:
  std::shared_ptr<ExprAST> cond_expression_;
  std::shared_ptr<Stmt> then_branch_;
  std::shared_ptr<Stmt> else_branch_;
};

class PrintStmt : public Stmt , std::enable_shared_from_this<PrintStmt>{
 public:
  explicit PrintStmt(std::shared_ptr<ExprAST> expr) : expr_(std::move(expr)){}
  auto GetExpr() const -> std::shared_ptr<ExprAST> { return expr_; }
 private:
  std::shared_ptr<ExprAST> expr_;
};

class WhileStmt : public Stmt, std::enable_shared_from_this<WhileStmt>{
public:
  WhileStmt(std::shared_ptr<ExprAST> cond_expression, std::shared_ptr<Stmt> body) : 
    cond_expression_(std::move(cond_expression)), body_(std::move(body)) {}
  auto GetConditionExpr() const -> std::shared_ptr<ExprAST> { return cond_expression_; }
  auto GetWhileBody() const -> std::shared_ptr<Stmt> { return body_; }
private:
  std::shared_ptr<ExprAST> cond_expression_;
  std::shared_ptr<Stmt> body_;
};

class BlockStmt : public Stmt, std::enable_shared_from_this<BlockStmt> {
public:
  explicit BlockStmt(std::vector<std::shared_ptr<Stmt>> stmts) : stmts_(std::move(stmts)) {}
  auto GetBlockStatements() const -> std::vector<std::shared_ptr<Stmt>> { return stmts_;}
private:
  std::vector<std::shared_ptr<Stmt>> stmts_;
};

class ExpressionStmt : public Stmt , std::enable_shared_from_this<ExpressionStmt>{
public:
  explicit ExpressionStmt(std::shared_ptr<ExprAST> expr) : expr_(std::move(expr)) {}
  auto GetExpr() const -> std::shared_ptr<ExprAST> { return expr_; }
private:
  std::shared_ptr<ExprAST> expr_;
};

class VarStmt : public Stmt, std::enable_shared_from_this<VarStmt> {
public:
  VarStmt(const Token &name, std::shared_ptr<ExprAST> expr) : name_(name), expr_(std::move(expr)){}
  auto GetExpr() const -> std::shared_ptr<ExprAST> { return expr_; }
  auto GetName() const -> Token { return name_; }
private:
  Token name_;
  std::shared_ptr<ExprAST> expr_;
};

class FunctionStmt : public Stmt, std::enable_shared_from_this<FunctionStmt> {
public:
  FunctionStmt(const Token &name, const std::vector<Token> &params, const std::vector<std::shared_ptr<Stmt>>& body) : 
                name_(name), params_(params), body_(body) {}
  auto GetFunctionParams() const -> std::vector<Token> { return params_; }
  auto GetFunctionBody() const -> std::vector<std::shared_ptr<Stmt>> { return body_; }
  auto GetFunctionName() const -> Token { return name_; }
private:
  Token name_;
  std::vector<Token> params_;
  std::vector<std::shared_ptr<Stmt>> body_;
};

class ReturnStmt : public Stmt {
public:
  ReturnStmt(const Token &keyword, std::shared_ptr<ExprAST> value) : keyword_(keyword), value_(std::move(value)){}
  auto GetReturnValue() const -> std::shared_ptr<ExprAST> { return value_; }
private:
  Token keyword_;
  std::shared_ptr<ExprAST> value_;
};

}  // namespace cpplox