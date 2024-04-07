#pragma once

#include <any>
#include <map>
#include <string>
#include <utility>

namespace cpplox {

enum class TokenType {
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,
  TOKEN_EOF
};

class Token {
 public:
  Token(TokenType token_type, std::string lexeme, std::any literal, int line)
      : token_type_(token_type), lexeme_(std::move(lexeme)), literal_(std::move(literal)),line_(line) {}
  Token(const Token &other) {
    this->token_type_ = other.token_type_;
    this->lexeme_ = other.lexeme_;
    this->line_ = other.line_;
  }
  // TODO(gaoxiang):
  // auto ToString() -> std::string {

  // }
  auto GetTokenType() const -> TokenType { return token_type_; }
  auto GetTokenLine() const -> int { return line_; }
  auto GetTokenLexeme() const -> std::string { return lexeme_; }
 private:
  TokenType token_type_;
  std::string lexeme_;
  int line_;
  std::any literal_;

 public:
  inline static std::map<std::string, TokenType> key_words{
      {"and", TokenType::AND},     {"class", TokenType::CLASS},   {"else", TokenType::ELSE},
      {"false", TokenType::FALSE}, {"for", TokenType::FOR},       {"fun", TokenType::FUN},
      {"if", TokenType::IF},       {"nil", TokenType::NIL},       {"or", TokenType::OR},
      {"print", TokenType::PRINT}, {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
      {"this", TokenType::THIS},   {"true", TokenType::TRUE},     {"var", TokenType::VAR},
      {"while", TokenType::WHILE}};
};

}  // namespace cpplox