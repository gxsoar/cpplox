#pragma once

#include "interpreter.h"
#include "lox_callable.h"

namespace cpplox {

class NativeClock : public LoxCallable {
public:
  auto Arity() -> int override { return 0;}
  auto Call(Interpreter &interpreter, std::vector<std::any> &arguments) -> std::any override {
    auto ticks = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration<double>{ticks}.count() / 1000.0;
  }
  auto ToString() -> std::string override { return "<native fn>";}
};

} // namespace cpplox