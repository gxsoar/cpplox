#pragma once

#include <any>
#include <stdexcept>
#include <utility>

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

class Return {
public:
  explicit Return(std::any value) : value_(std::move(value)) {}
  auto GetReturnValue() const -> std::any { return value_; }
private:
  std::any value_;
};

} // namespace cpplox