#pragma once

#include <any>
#include <vector>
#include "interpreter.h"
namespace cpplox {

class LoxCallable {
public:
  virtual auto Call(Interpreter &interpreter, std::vector<std::any> &arguments) -> std::any = 0;
  virtual auto Arity() -> int = 0;
  virtual auto ToString() -> std::string = 0;
};

} // namespace cpplox