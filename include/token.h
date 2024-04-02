#pragma once

#include <map>
#include <string>
#include <utility>

namespace cpplox {

enum class TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

  // One or two character tokens.
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,

  // Literals.
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  AND, CLASS, ELSE, FALSE, FUN, FOR, IF, NIL, OR,
  PRINT, RETURN, SUPER, THIS, TRUE, VAR, WHILE,
  TOKEN_EOF
};

class Token {
public:
  Token(TokenType token_type, std::string lexeme, int line) : 
    token_type_(token_type), lexeme_(std::move(lexeme)), line_(line) {}
  // TODO(gaoxiang): 
  // auto ToString() -> std::string {
    
  // }
private:
  TokenType token_type_;
  std::string lexeme_;
  int line_;
public:
  static std::map<std::string, TokenType> key_words;
};

}  // namespace cpplox