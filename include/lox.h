#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "error.h"
#include "interpreter.h"
#include "scanner.h"
#include "token.h"

namespace cpplox {

class Lox {
public:
  auto RunFile(const std::string& filePath) -> void;
  auto RunPrompt() -> void; 
 
private:
  auto Run(const std::string& source) -> void;
  inline static std::shared_ptr<Interpreter> interpreter{std::make_shared<Interpreter>()};
};

}  // namespace cpplox