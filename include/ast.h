#pragma once

class ExprAST {
public:
  virtual ~ExprAST() = default;
};

class BinaryExprAST : public ExprAST {

};

class UnaryExprAST : public ExprAST {

};

class LiteralExprAST : public ExprAST {
  
};

class GroupingExprAST : public ExprAST {

};