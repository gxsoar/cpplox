#pragma once

#include <token.h>
#include <exception>
#include <iostream>
#include <string>
#include "runtime_error.h"

namespace cpplox {

inline bool had_error {false};
inline bool had_runtime_error{false};

class Log {
 public:
  void Error(int line, const std::string &message) {
    Report(line, "const std::string &where", message);
  }
  static void Error(const Token &token, const std::string &message) {
    if (token.GetTokenType() == TokenType::TOKEN_EOF) {
      Report(token.GetTokenLine(), "at end", message);
    } else {
      Report(token.GetTokenLine(), " at " + token.GetTokenLexeme() + "`",message);
    }
  }
  static void RuntimeError(const RuntimeError &error) {
    std::cerr << error.what() << "\n[line " << error.GetToken().GetTokenLine() << "]\n";
    had_error = true;
  }
 private:
  static void Report(int line, const std::string &where, const std::string &message) {
    std::cerr << "[line " << line << "] Error " << where << " : " << message << "\n";
    had_error = true;
  }
};

}  // namespace cpplox