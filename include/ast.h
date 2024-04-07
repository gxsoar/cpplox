#pragma once

#include <algorithm>
#include <any>
#include <memory>
#include <utility>

#include "token.h"
namespace cpplox {

class BinaryExprAST;
class GroupingExprAST;
class LiteralExprAST;
class UnaryExprAST;

class ExprASTVisitor {
public:
  virtual auto VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitGroupingExprAST(std::shared_ptr<GroupingExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitLiteralExprAST(std::shared_ptr<LiteralExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any = 0;
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

} // namespace cpplox