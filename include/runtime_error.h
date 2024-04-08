#pragma once

#include <stdexcept>

#include "token.h"

namespace cpplox {

class RuntimeError : public std::runtime_error {
public:
  explicit RuntimeError(const Token &token, const std::string &message) : 
    token_(token), std::runtime_error{message} {}
  auto GetToken() const -> Token { return token_; }
private:
  Token token_;
};

} // namespace cpplox