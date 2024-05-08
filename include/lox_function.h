#pragma once

#include <algorithm>
#include <any>
#include <memory>
#include <string>
#include <vector>
#include "environment.h"
#include "interpreter.h"
#include "lox_callable.h"
#include "runtime_error.h"
#include "stmt.h"
namespace cpplox {

class LoxFunction : public LoxCallable {
public:
  explicit LoxFunction(std::shared_ptr<FunctionStmt> declaration, const Environment &closure) : 
            declaration_(std::move(declaration)), closure_(closure) {}
  auto Call(Interpreter &interpreter, std::vector<std::any> &arguments) -> std::any override {
    auto environment {std::make_shared<Environment>(closure_)};
    for (int i = 0; i < declaration_->GetFunctionParams().size(); ++ i) {
      environment->Define(declaration_->GetFunctionParams()[i].GetTokenLexeme(), arguments[i]);
    }
    try {
      interpreter.ExecuteBlock(declaration_->GetFunctionBody(), environment);
    } catch(Return return_value) {
      return return_value.GetReturnValue();
    }
    
    return {};
  }
  auto Arity() -> int override { return declaration_->GetFunctionParams().size(); }
  auto ToString() -> std::string override { return "<fn" + declaration_->GetFunctionName().GetTokenLexeme() + ">"; }
private:
  std::shared_ptr<FunctionStmt> declaration_;
  Environment closure_;
};

} // namespace cpplox