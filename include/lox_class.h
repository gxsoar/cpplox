#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include "lox_callable.h"
#include "lox_function.h"
#include "lox_instance.h"

namespace cpplox {

class LoxClass : public LoxCallable {
public:
  explicit LoxClass(std::string name, const std::unordered_map<std::string, std::shared_ptr<LoxFunction>> &methods) : 
            name_(std::move(name)),
            methods_(methods) {}
  LoxClass(const LoxClass &rhs) {
    name_ = rhs.name_;
  }
  auto ToString() const -> std::string { return name_; }
  auto Arity() -> int override { 
    auto initializer {FindMethod("init")};
    if (initializer == nullptr) {
      return 0;
    }
    return initializer->Arity();
  }
  auto Call(Interpreter &interpreter, std::vector<std::any> &arguments) -> std::any override {
    std::shared_ptr<LoxInstance> instance = std::make_shared<LoxInstance>(this);
    auto initializer {FindMethod("init")};
    if (initializer != nullptr) {
      initializer->Bind(instance)->Call(interpreter, arguments);
    }
    return instance;
  }
  auto FindMethod(const std::string &method_name) -> std::shared_ptr<LoxFunction> { 
    if (methods_.contains(method_name)) {
      return methods_[method_name];
    }
    return nullptr;
  }
private:
  std::string name_;
  std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods_;
};

} // namespace cpplox