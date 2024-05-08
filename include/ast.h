#pragma once

#include <algorithm>
#include <any>
#include <memory>
#include <utility>
#include <vector>

#include "token.h"
namespace cpplox {

class BinaryExprAST;
class GroupingExprAST;
class LiteralExprAST;
class UnaryExprAST;
class LogicalExprAST;
class VarExprAST;
class AssignExprAST;
class CallExprAST;

class ExprASTVisitor {
public:
  virtual auto VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitLogicalExprAST(std::shared_ptr<LogicalExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitVariableExprAST(std::shared_ptr<VarExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitAssignmentExprAST(std::shared_ptr<AssignExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitCallExprAST(std::shared_ptr<CallExprAST> expr_ast) -> std::any = 0;
  virtual ~ExprASTVisitor() = default;
};

class ExprAST {
public:
  virtual auto Accept(ExprASTVisitor& visitor) -> std::any = 0;
  virtual ~ExprAST() = default;
};

class BinaryExprAST : public ExprAST, std::enable_shared_from_this<BinaryExprAST>{
public:
  BinaryExprAST(std::shared_ptr<ExprAST> left, const Token &op, std::shared_ptr<ExprAST> right) : 
    left_(std::move(left)), op_(op), right_(std::move(right)){}

  auto Accept(ExprASTVisitor &visitor) -> std::any override {
    return visitor.VisitBinaryExprAST(shared_from_this());
  }
  auto GetLeftExpr() const -> std::shared_ptr<ExprAST> { return left_; }
  auto GetRightExpr() const -> std::shared_ptr<ExprAST> { return right_; }
  auto GetOperation() const -> Token { return op_; }
private:
  std::shared_ptr<ExprAST> left_;
  std::shared_ptr<ExprAST> right_;
  Token op_;
};

class UnaryExprAST : public ExprAST, std::enable_shared_from_this<UnaryExprAST> {
public:
  UnaryExprAST(std::shared_ptr<ExprAST> right, const Token &op) : right_(std::move(right)), op_(op){}
  auto Accept(ExprASTVisitor &visitor) -> std::any override {
    return visitor.VisitUnaryExprAST(shared_from_this());
  }
  auto GetOperation() const -> Token { return op_; }
  auto GetRightExpr() const -> std::shared_ptr<ExprAST> { return right_; }
private:
  std::shared_ptr<ExprAST> right_;
  Token op_;
};

class LiteralExprAST : public ExprAST, std::enable_shared_from_this<LiteralExprAST> {
public:
  explicit LiteralExprAST(std::any value) : value_(std::move(value)) {}
  auto Accept(ExprASTVisitor &visitor) -> std::any override {
    return visitor.VisitLiteralExprAST(shared_from_this());
  }
  auto GetValue() const -> std::any { return value_; }
private:
  std::any value_;
};

class GroupingExprAST : public ExprAST, std::enable_shared_from_this<GroupingExprAST> {
public:
  explicit GroupingExprAST(std::shared_ptr<ExprAST> expression) : expression_(std::move(expression)) {}
  auto Accept(ExprASTVisitor &visitor) -> std::any override {
    return visitor.VisitGroupingExprAST(shared_from_this());
  }
  auto GetExpression() const -> std::shared_ptr<ExprAST> { return expression_; }
private:
  std::shared_ptr<ExprAST> expression_;
};

class LogicalExprAST : public ExprAST, std::enable_shared_from_this<LogicalExprAST> {
public:
  explicit LogicalExprAST(std::shared_ptr<ExprAST> left, const Token& op, std::shared_ptr<ExprAST> right) :
    left_(std::move(left)), op_(op), right_(std::move(right)) {}
  auto GetLeftExpr() const -> std::shared_ptr<ExprAST> { return left_; }
  auto GetRightExpr() const -> std::shared_ptr<ExprAST> { return  right_;}
  auto GetToken() const -> Token { return op_;}
private:
  std::shared_ptr<ExprAST> left_;
  Token op_;
  std::shared_ptr<ExprAST> right_;
};

class VarExprAST : public ExprAST, std::enable_shared_from_this<VarExprAST> {
public:
  explicit VarExprAST(const Token &op) : op_(op) {}
  auto GetToken() const -> Token { return op_; }
private:
  Token op_;
};

class AssignExprAST : public ExprAST, std::enable_shared_from_this<AssignExprAST> {
public:
  explicit AssignExprAST(const Token &name, std::shared_ptr<ExprAST> value) : name_(name), value_(std::move(value)) {}
  auto Accept(ExprASTVisitor &visitor) -> std::any override {
    return visitor.VisitAssignmentExprAST(shared_from_this());
  }
  auto GetValue() const -> std::shared_ptr<ExprAST> { return value_; }
  auto GetName() const -> Token { return name_; }
private:
  Token name_;
  std::shared_ptr<ExprAST> value_;
};

class CallExprAST : public ExprAST, std::enable_shared_from_this<CallExprAST> {
public:
  explicit CallExprAST(std::shared_ptr<ExprAST> callee, const Token &op, const std::vector<std::shared_ptr<ExprAST>>& arguments) : 
                        callee_(std::move(callee)),op_(op), arguments_(arguments){}
  auto GetCallee() const -> std::shared_ptr<ExprAST> { return callee_;}
  auto GetArguments() const -> std::vector<std::shared_ptr<ExprAST>> { return arguments_; }
  auto GetToken() const -> Token { return op_; }
private:
  std::shared_ptr<ExprAST> callee_;
  Token op_;
  std::vector<std::shared_ptr<ExprAST>> arguments_;
};


} // namespace cpplox