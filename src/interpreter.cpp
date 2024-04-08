#include "interpreter.h"
#include <any>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>
#include <iostream>

#include "ast.h"
#include "runtime_error.h"
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

}  // namespace cpplox