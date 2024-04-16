#pragma once

#include <any>
#include <future>
#include <memory>
#include <unordered_map>
#include <string>
#include "runtime_error.h"
#include "token.h"

namespace cpplox {

class Environment : public std::enable_shared_from_this<Environment>{
public:
  Environment() : enclosing_(nullptr) {}
  explicit Environment(std::shared_ptr<Environment> enclosing) : enclosing_(std::move(enclosing)) {}
  void Define(const std::string &name, const std::any &value) { values_[name] = value; }

  auto Get(const Token &name) -> std::any {
    if (values_.contains(name.GetTokenLexeme())) {
      return values_[name.GetTokenLexeme()];
    }
    if (enclosing_ != nullptr) {
      return enclosing_->Get(name);
    }
    throw RuntimeError(name, "Undefined variable" + name.GetTokenLexeme() + ".");
  }

  void Assign(const Token &name, const std::any &value) {
    if (values_.contains(name.GetTokenLexeme())) {
      values_[name.GetTokenLexeme()] = value;
      return;
    }
    if (enclosing_ != nullptr) {
      enclosing_->Assign(name, value);
      return;
    }
    throw RuntimeError(name, "Undefined variable " + name.GetTokenLexeme() + ".");
  }

private:
  std::unordered_map<std::string, std::any> values_;
  std::shared_ptr<Environment> enclosing_;
};

} // namespace cpplox