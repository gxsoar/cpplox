#pragma once

#include <algorithm>
#include <any>
#include <memory>
#include <string>
#include <vector>
#include "environment.h"
#include "interpreter.h"
#include "lox_callable.h"
#include "lox_instance.h"
#include "runtime_error.h"
#include "stmt.h"
namespace cpplox {

class LoxFunction : public LoxCallable {
public:
  explicit LoxFunction(std::shared_ptr<FunctionStmt> declaration, const Environment &closure, bool is_initializer) : 
            declaration_(std::move(declaration)), closure_(closure), is_initializer_(is_initializer) {}
  auto Call(Interpreter &interpreter, std::vector<std::any> &arguments) -> std::any override {
    auto environment {std::make_shared<Environment>(closure_)};
    for (int i = 0; i < declaration_->GetFunctionParams().size(); ++ i) {
      environment->Define(declaration_->GetFunctionParams()[i].GetTokenLexeme(), arguments[i]);
    }
    try {
      interpreter.ExecuteBlock(declaration_->GetFunctionBody(), environment);
    } catch(Return return_value) {
      if (is_initializer_) { return closure_.GetAt(0, "this"); }
      return return_value.GetReturnValue();
    }
    if (is_initializer_) { return closure_.GetAt(0, "this"); }
    return {};
  }
  auto Arity() -> int override { return declaration_->GetFunctionParams().size(); }
  auto ToString() -> std::string override { return "<fn" + declaration_->GetFunctionName().GetTokenLexeme() + ">"; }
  auto Bind(const std::shared_ptr<LoxInstance> &instance) -> std::shared_ptr<LoxFunction> {
    auto environment {std::make_shared<Environment>(closure_)};
    environment->Define("this", instance);
    return std::make_shared<LoxFunction>(declaration_, environment, is_initializer_);
  }
private:
  std::shared_ptr<FunctionStmt> declaration_;
  Environment closure_;
  bool is_initializer_;
};

} // namespace cpplox