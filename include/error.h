#pragma once

#include <token.h>
#include <iostream>
#include <string>

namespace cpplox {

inline bool had_error {false};

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

 private:
  static void Report(int line, const std::string &where, const std::string &message) {
    std::cerr << "[line " << line << "] Error " << where << " : " << message << "\n";
    had_error = true;
  }
};

}  // namespace cpplox