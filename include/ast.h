#pragma once

#include <algorithm>
#include <any>
#include <future>
#include <memory>
#include <utility>
#include <vector>

#include "stmt.h"
#include "token.h"
namespace cpplox {

class ExprAST;
class BinaryExprAST;
class GroupingExprAST;
class LiteralExprAST;
class UnaryExprAST;
class LogicalExprAST;
class VarExprAST;
class AssignExprAST;
class CallExprAST;
class GetExprAST;
class SetExprAST;
class ThisExprAST;
class SuperExprAST;

using ExprASTPtr = std::shared_ptr<ExprAST>;
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
  virtual auto VisitGetExprAST(std::shared_ptr<GetExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitSetExprAST(std::shared_ptr<SetExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitThisExprAST(std::shared_ptr<ThisExprAST> expr_ast) -> std::any = 0;
  virtual auto VisitSuperExprAST(std::shared_ptr<SuperExprAST> expr_ast) -> std::any = 0;
  virtual ~ExprASTVisitor() = default;
};

class ExprAST {
 public:
  virtual auto Accept(ExprASTVisitor &visitor) -> std::any = 0;
  virtual ~ExprAST() = default;
};

class BinaryExprAST : public ExprAST, std::enable_shared_from_this<BinaryExprAST> {
 public:
  BinaryExprAST(ExprASTPtr left, const Token &op, ExprASTPtr right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}

  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitBinaryExprAST(shared_from_this()); }
  auto GetLeftExpr() const -> ExprASTPtr { return left_; }
  auto GetRightExpr() const -> ExprASTPtr { return right_; }
  auto GetOperation() const -> Token { return op_; }

 private:
  ExprASTPtr left_;
  ExprASTPtr right_;
  Token op_;
};

class UnaryExprAST : public ExprAST, std::enable_shared_from_this<UnaryExprAST> {
 public:
  UnaryExprAST(ExprASTPtr right, const Token &op) : right_(std::move(right)), op_(op) {}
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitUnaryExprAST(shared_from_this()); }
  auto GetOperation() const -> Token { return op_; }
  auto GetRightExpr() const -> ExprASTPtr { return right_; }

 private:
  ExprASTPtr right_;
  Token op_;
};

class LiteralExprAST : public ExprAST, std::enable_shared_from_this<LiteralExprAST> {
 public:
  explicit LiteralExprAST(std::any value) : value_(std::move(value)) {}
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitLiteralExprAST(shared_from_this()); }
  auto GetValue() const -> std::any { return value_; }

 private:
  std::any value_;
};

class GroupingExprAST : public ExprAST, std::enable_shared_from_this<GroupingExprAST> {
 public:
  explicit GroupingExprAST(ExprASTPtr expression) : expression_(std::move(expression)) {}
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitGroupingExprAST(shared_from_this()); }
  auto GetExpression() const -> ExprASTPtr { return expression_; }

 private:
  ExprASTPtr expression_;
};

class LogicalExprAST : public ExprAST, std::enable_shared_from_this<LogicalExprAST> {
 public:
  explicit LogicalExprAST(ExprASTPtr left, const Token &op, ExprASTPtr right)
      : left_(std::move(left)), op_(op), right_(std::move(right)) {}
  auto GetLeftExpr() const -> ExprASTPtr { return left_; }
  auto GetRightExpr() const -> ExprASTPtr { return right_; }
  auto GetToken() const -> Token { return op_; }
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitLogicalExprAST(shared_from_this()); }

 private:
  ExprASTPtr left_;
  Token op_;
  ExprASTPtr right_;
};

class VarExprAST : public ExprAST, std::enable_shared_from_this<VarExprAST> {
 public:
  explicit VarExprAST(const Token &op) : op_(op) {}
  auto GetToken() const -> Token { return op_; }
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitVariableExprAST(shared_from_this()); }

 private:
  Token op_;
};

class AssignExprAST : public ExprAST, std::enable_shared_from_this<AssignExprAST> {
 public:
  explicit AssignExprAST(const Token &name, ExprASTPtr value) : name_(name), value_(std::move(value)) {}
  auto Accept(ExprASTVisitor &visitor) -> std::any override {
    return visitor.VisitAssignmentExprAST(shared_from_this());
  }
  auto GetValue() const -> ExprASTPtr { return value_; }
  auto GetName() const -> Token { return name_; }

 private:
  Token name_;
  ExprASTPtr value_;
};

class CallExprAST : public ExprAST, std::enable_shared_from_this<CallExprAST> {
 public:
  explicit CallExprAST(ExprASTPtr callee, const Token &op, const std::vector<ExprASTPtr> &arguments)
      : callee_(std::move(callee)), op_(op), arguments_(arguments) {}
  auto GetCallee() const -> ExprASTPtr { return callee_; }
  auto GetArguments() const -> std::vector<ExprASTPtr> { return arguments_; }
  auto GetToken() const -> Token { return op_; }
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitCallExprAST(shared_from_this()); }

 private:
  ExprASTPtr callee_;
  Token op_;
  std::vector<ExprASTPtr> arguments_;
};

class GetExprAST : public ExprAST, std::enable_shared_from_this<GetExprAST> {
 public:
  explicit GetExprAST(ExprASTPtr object, const Token &name) : object_(std::move(object)), name_(name) {}
  auto GetObject() const -> ExprASTPtr { return object_; }
  auto GetName() const -> Token { return name_; }
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitGetExprAST(shared_from_this()); }
 private:
  ExprASTPtr object_;
  Token name_;
};

class SetExprAST : public ExprAST, std::enable_shared_from_this<SetExprAST> {
 public:
  explicit SetExprAST(ExprASTPtr object, const Token &name, ExprASTPtr value)
      : object_(std::move(object)), name_(name), value_(std::move(value)) {}
  auto GetSetObject() const -> ExprASTPtr { return object_; }
  auto GetSetName() const -> Token { return name_; }
  auto GetSetValue() const -> ExprASTPtr { return value_; }
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitSetExprAST(shared_from_this()); }
 private:
  ExprASTPtr object_;
  Token name_;
  ExprASTPtr value_;
};

class ThisExprAST : public ExprAST, std::enable_shared_from_this<ThisExprAST> {
public:
  explicit ThisExprAST(const Token &keyword) : keyword_(keyword) {}
  auto GetThisKeyWord() const -> Token { return keyword_; }
  auto Accept(ExprASTVisitor &visitor) -> std::any override { return visitor.VisitThisExprAST(shared_from_this()); }
private:
  Token keyword_;
};

class SuperExprAST : public ExprAST, std::enable_shared_from_this<SetExprAST> {
public:
  explicit SuperExprAST(const Token &keyword, const Token &method) : keyword_(keyword), method_(method) {}
  auto GetSuperkeyWord() const { return keyword_; }
  auto GetSuperMethod() const { return method_; }
private:
  Token keyword_;
  Token method_;
};

}  // namespace cpplox