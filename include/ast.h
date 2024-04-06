#pragma once

#include <algorithm>
#include <memory>

#include "token.h"
namespace cpplox {

class ExprAST {
public:
  virtual ~ExprAST() = default;
};

class BinaryExprAST : public ExprAST {
public:
  BinaryExprAST(std::shared_ptr<ExprAST> left, const Token &op, std::shared_ptr<ExprAST> right) : 
    left_(std::move(left)), op_(op), right_(std::move(right)){}
private:
  std::shared_ptr<ExprAST> left_;
  std::shared_ptr<ExprAST> right_;
  Token op_;
};

class UnaryExprAST : public ExprAST {
public:
  UnaryExprAST(std::shared_ptr<ExprAST> right, const Token &op) : right_(std::move(right)), op_(op){}
private:
  std::shared_ptr<ExprAST> right_;
  Token op_;
};

class LiteralExprAST : public ExprAST {
  
};

class GroupingExprAST : public ExprAST {
public:
  explicit GroupingExprAST(std::shared_ptr<ExprAST> left) : left_(std::move(left)) {}
private:
  std::shared_ptr<ExprAST> left_;
};

} // namespace cpplox