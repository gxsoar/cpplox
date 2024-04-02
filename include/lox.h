#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "scanner.h"
#include "token.h"

namespace cpplox {

class Lox {
public:
  auto RunFile(const std::string& filePath) -> void;
  auto RunPrompt() -> void; 
  auto Error(int line, const std::string &message)->void; 
private:
  auto Run(const std::string& source) -> void; 
  auto Report(int line, const std::string &where, const std::string &message) -> void;
private:
  bool had_error_{false};
};

}  // namespace cpplox