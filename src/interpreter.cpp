#include "interpreter.h"
#include <any>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>
#include <vector>

#include "ast.h"
#include "lox_callable.h"
#include "lox_function.h"
#include "runtime_error.h"
#include "stmt.h"
#include "token.h"
#include "error.h"

namespace cpplox {

auto Interpreter::VisitUnaryExprAST(std::shared_ptr<UnaryExprAST> expr_ast) -> std::any  {
  auto right{Evaluate(expr_ast->GetRightExpr())};
  switch (expr_ast->GetOperation().GetTokenType()) {
    case TokenType::MINUS:
      return -std::any_cast<double>(right);
    case TokenType::BANG:
      return !std::any_cast<bool>(IsTruthy(right));
    default:
      break;
  }
  return nullptr;
}

auto Interpreter::IsTruthy(const std::any &value) -> bool {
  auto &value_type = value.type();
  if (value_type == typeid(nullptr)) {
    return false;
  }
  if (value_type == typeid(bool)) {
    return std::any_cast<bool>(value);
  }
  return true;
}

auto Interpreter::VisitBinaryExprAST(std::shared_ptr<BinaryExprAST> expr_ast) -> std::any {
  auto left {Evaluate(expr_ast)};
  auto right {Evaluate(expr_ast)};
  auto op {expr_ast->GetOperation()};
  switch (expr_ast->GetOperation().GetTokenType()) {
    case TokenType::GREATER:
      CheckNumberOperand(op, left, right);
      return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
      CheckNumberOperand(op, left, right);
      return std::any_cast<double>(left) >= std::any_cast<double>(right);
    case TokenType::LESS:
      CheckNumberOperand(op, left, right);
      return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
      CheckNumberOperand(op, left, right);
      return std::any_cast<double>(left) <= std::any_cast<double>(right);
    case TokenType::BANG_EQUAL:
      return !IsEqual(left, right);
    case TokenType::EQUAL_EQUAL:
      return IsEqual(left, right);
    case TokenType::MINUS:
      CheckNumberOperand(op, left, right);
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::PLUS:
      if (left.type() == typeid(double) && right.type() == typeid(double)) {
        return std::any_cast<double>(left) + std::any_cast<double>(right);
      }
      if (left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
        return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
      }
      throw RuntimeError(op, "Operands must be two numbers or two strings");
      break;
    case TokenType::SLASH:
      CheckNumberOperand(op, left, right);
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
      CheckNumberOperand(op, left, right);
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    default:
      break;
  }

  return nullptr;
}

auto Interpreter::IsEqual(const std::any &left, const std::any &right) -> bool {
  if (left.type() == typeid(nullptr) && right.type() == typeid(nullptr)) {
    return true;
  }
  if (left.type() == typeid(nullptr)) {
    return false;
  }
  if (left.type() == right.type()) {
    if (left.type() == typeid(std::string)) {
      return std::any_cast<std::string>(left) == std::any_cast<std::string>(right);
    }
    if (left.type() == typeid(double)) {
      return std::any_cast<double>(left) == std::any_cast<double>(right);
    }
  }
  return false;
}

void Interpreter::CheckNumberOperand(const Token &op, const std::any &left, const std::any &right) {
  if (left.type() == typeid(double) && right.type() == typeid(double)) {
    return;
  }
  throw RuntimeError(op, "Operand must be a number");
}

void Interpreter::Interpret(const std::shared_ptr<ExprAST> &expression) {
  try {
  auto value {Evaluate(expression)};
  std::cout << StringIfy(value) << "\n";
  } catch (RuntimeError error) {
    Log::RuntimeError(error);
  }
}

void Interpreter::Interpret(const std::vector<std::shared_ptr<Stmt>> &statements) {
  try {
  for (const auto& statement : statements) {
    Execute(statement);
  }
  } catch (RuntimeError error) {
    Log::RuntimeError(error);
  }
}

auto Interpreter::StringIfy(const std::any &value) -> std::string {
  if (value.type() == typeid(nullptr)) {
    return "nil";
  }
  if (value.type() == typeid(double)) {
    auto text {std::any_cast<std::string>(value)};
    if (text.ends_with(".0")) {
      text = text.substr(0, text.length() - 2);
    }
    return text;
  }
  return std::any_cast<std::string>(value);
}

void Interpreter::VisitIfStmt(std::shared_ptr<IfStmt> stmt) {
  // 对表达式进行求值，如果为真执行then_branch否则执行else_branch
  if (IsTruthy(Evaluate(stmt->GetConditionExpression()))) {
    Execute(stmt->GetThenBranch());
  } else if (stmt->GetElseBranch() != nullptr) {
    Execute(stmt->GetElseBranch());
  }
}

auto Interpreter::VisitLogicalExprAST(std::shared_ptr<LogicalExprAST> expr_ast) -> std::any {
  auto left {Evaluate(expr_ast)};
  if (expr_ast->GetToken().GetTokenType() == TokenType::OR) {
    if (IsTruthy(left)) {
      return left;
    }
  } else {
    if (!IsTruthy(left)) {
      return left;
    }
  }

  return Evaluate(expr_ast->GetRightExpr());
}

void Interpreter::VisitWhileStmt(std::shared_ptr<WhileStmt> stmt) {
  while(IsTruthy(stmt->GetConditionExpr())) {
    Execute(stmt->GetWhileBody());
  }
}

void Interpreter::VisitExpressionStmt(std::shared_ptr<ExpressionStmt> stmt) {
  Evaluate(stmt->GetExpr());
}

void Interpreter::VisitPrintStmt(std::shared_ptr<PrintStmt> stmt) {
  auto value {stmt->GetExpr()};
  std::cout << StringIfy(value) << "\n";
}

void Interpreter::Execute(const std::shared_ptr<Stmt> &stmt) {
  stmt->Accept(*this);
}

void Interpreter::VisitVarStmt(std::shared_ptr<VarStmt> stmt) {
  std::any value;
  if (stmt->GetExpr() != nullptr) {
    value = Evaluate(stmt->GetExpr());
  }
  environment_->Define(stmt->GetName().GetTokenLexeme(), value);
}

auto Interpreter::VisitAssignmentExprAST(std::shared_ptr<AssignExprAST> expr_ast) -> std::any {
  auto value {Evaluate(expr_ast->GetValue())};
  environment_->Assign(expr_ast->GetName(), value);
  return value;
}

void Interpreter::VisitFunctionStmt(std::shared_ptr<FunctionStmt> stmt) {
  auto function {std::make_shared<FunctionStmt>(stmt, environment_)};
  environment_->Define(stmt->GetFunctionName().GetTokenLexeme(), function);
}

void Interpreter::VisitReturnStmt(std::shared_ptr<ReturnStmt> stmt) {
  std::any value;
  if (stmt->GetReturnValue() != nullptr) {
    value = Evaluate(stmt->GetReturnValue());
  }
  throw Return(value);
}

void Interpreter::ExecuteBlock(const std::vector<std::shared_ptr<Stmt>> &statements, const std::shared_ptr<Environment> &env) {
  auto previous = this->environment_;
  try {
    this->environment_ = env;
    for (const auto &statement : statements) {
      Execute(statement);
    }
  } catch(...) {
    this->environment_ = previous;
    throw;
  }
  this->environment_ = previous;
}

auto Interpreter::VisitCallExprAST(std::shared_ptr<CallExprAST> expr_ast) -> std::any {
  auto callee {Evaluate(expr_ast->GetCallee())};
  std::vector<std::any> arguments;
  for (const auto &argument : expr_ast->GetArguments()) {
    arguments.emplace_back(Evaluate(argument));
  }
  if (callee.type() == typeid(LoxCallable)) {
    throw RuntimeError{expr_ast->GetToken(), "Can only call functions and classes."};
  }
  auto function {std::any_cast<LoxFunction>(callee)};
  if (arguments.size() != function.Arity()) {
    std::string message = "Expected "; 
    message += (std::to_string(function.Arity()) + " arguments but got " + std::to_string(arguments.size()) + ".");
    throw RuntimeError{expr_ast->GetToken(), message};
  }
  return function.Call(*this, arguments);
}

void Interpreter::Resolve(const std::shared_ptr<ExprAST> &expr, int depth) {
  locals_[expr] = depth;
}

auto Interpreter::LookUpVariable(const Token &name, const std::shared_ptr<ExprAST> &expr) -> std::any {
  if (locals_.contains(expr)) {
    return environment_->GetAt(locals_[expr], name.GetTokenLexeme());
  }      
  return globals_->Get(name);
}

}  // namespace cpplox