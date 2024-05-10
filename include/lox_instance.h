#pragma once

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include "lox_class.h"
#include "runtime_error.h"
#include "token.h"
namespace cpplox {

class LoxInstance {
public:
  explicit LoxInstance(const std::shared_ptr<LoxClass>& klass) : klass_(klass) {}
  auto ToString() -> std::string { return klass_->ToString() + " instance"; }
  auto Get(const Token &name) -> std::any {
    if (fields_.contains(name.GetTokenLexeme())) {
      return fields_[name.GetTokenLexeme()];
    }
    auto method {klass_->FindMethod(name.GetTokenLexeme())};
    if (method != nullptr) { return method->Bind(this); }
    throw RuntimeError(name, "Undefined property " + name.GetTokenLexeme() + " .");
  }
  void Set(const Token &name, const std::any &value) {
    fields_[name.GetTokenLexeme()] = value;
  }
private:
  std::shared_ptr<LoxClass> klass_;
  std::unordered_map<std::string, std::any> fields_;
};
} // namespace cpplox