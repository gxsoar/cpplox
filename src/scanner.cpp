#include "scanner.h"
#include <cctype>
#include <list>
#include "token.h"
#include "lox.h"

namespace cpplox {

auto Scanner::ScanTokens() -> std::list<Token> {
  while(!IsAtEnd()) {
    start_ = current_;
    ScanToken();
  }

  tokens_.emplace_back(TokenType::TOKEN_EOF, "", line_);
  return tokens_;
}

auto Scanner::ScanToken() -> void {
  auto ch = Advance();
  switch (ch) {
    case '(':
      AddToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      AddToken(TokenType::RIGHT_PAREN);
      break;
    case '{':
      AddToken(TokenType::LEFT_BRACE);
      break;
    case '}':
      AddToken(TokenType::RIGHT_BRACE);
      break;
    case ',':
      AddToken(TokenType::COMMA);
      break;
    case '.':
      AddToken(TokenType::DOT);
      break;
    case '-':
      AddToken(TokenType::MINUS);
      break;
    case '+':
      AddToken(TokenType::PLUS);
      break;
    case '*':
      AddToken(TokenType::STAR);
      break;
    // deal with ! != = == < <= > >=
    case '!':
      AddToken(Match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '=':
      AddToken(Match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '<':
      AddToken(Match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      AddToken(Match('>') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case ';':
      AddToken(TokenType::SEMICOLON);
      break;
    // deal with comment //
    case '/':
      if (Match('/')) {
        while(Peek() != '\n' && !IsAtEnd()) { 
          Advance();
        }
      } else {
        AddToken(TokenType::SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      line_++;
      break;
    // deal with const string
    case '"':
      String();
      break;
    // TODO(gaoxiang): deal with not defined operator
    default:
      if (std::isdigit(ch) != 0) {
        Number();
      } else if(isalpha(ch) != 0) {
        Identifier();
      } else {
        throw "not defined operator";
      }
      
      break;
  }
}

auto Scanner::IsAtEnd() -> bool {
  return current_ >= source_.length();
}

auto Scanner::Advance() -> char {
  current_++;
  return source_[current_ - 1];
}

auto Scanner::AddToken(TokenType token_type) -> void {
  // get a complete token
  std::string text = source_.substr(start_, current_);
  tokens_.emplace_back(token_type, text, line_);
}

auto Scanner::AddToken(TokenType token_type, const std::string &val) -> void {
  tokens_.emplace_back(token_type, val, line_);
}

auto Scanner::Match(char expected) -> bool {
  if (IsAtEnd()) {
    return false;
  }
  if (source_[current_] != expected) {
    return false;
  }
  current_++;
  return true;
}

auto Scanner::Peek() -> char {
  if (IsAtEnd()) {
    return '\0';
  }
  return source_[current_];
}

auto Scanner::String() -> void {
  while(Peek() != '"' && !IsAtEnd()) {
    if (Peek() == '\n') {
      line_++;
    }
    Advance();
  }

  if (IsAtEnd()) {
    // TODO(gaoxiang): error
    return;
  }
  Advance();

  // get the const string
  std::string val = source_.substr(start_ + 1, current_ - 1);
  AddToken(TokenType::STRING, val);
}

auto Scanner::Number() -> void {
  while(isdigit(Peek()) != 0) {
    Advance();
  }
  // 处理小数点
  if (Peek() == '.' && (isdigit(PeekNext()) != 0)) {
    Advance();
    while(isdigit(Peek()) != 0) {
      Advance();
    }
  }
  AddToken(TokenType::NUMBER, source_.substr(start_, current_));
}

auto Scanner::PeekNext() -> char {
  if (current_ + 1 >= source_.length()) {
    return '\0';
  }
  return source_[current_ + 1];
}

auto Scanner::Identifier() -> void {
  while(IsAlphaNumeric(Peek())) {
    Advance();
  }
  std::string text = source_.substr(start_, current_);
  TokenType type;
  if (!Token::key_words.contains(text)) {
    type = TokenType::IDENTIFIER;
  } else {
    type = Token::key_words[text];
  }
  AddToken(TokenType::IDENTIFIER);
}

auto Scanner::IsAlphaNumeric(char ch) -> bool {
  return (isalpha(ch) != 0) || (isdigit(ch) != 0);
}

}  // namespace cpplox