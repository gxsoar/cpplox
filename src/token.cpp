#include "token.h"

namespace cpplox {

 std::map<std::string, TokenType> Token::key_words{{"and", TokenType::AND}, {"class", TokenType::CLASS},
                                                {"else", TokenType::ELSE}, {"false", TokenType::FALSE}, 
                                                {"for", TokenType::FOR}, {"fun", TokenType::FUN},
                                                {"if", TokenType::IF}, {"nil", TokenType::NIL},
                                                {"or", TokenType::OR}, {"print", TokenType::PRINT},
                                                {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
                                                {"this", TokenType::THIS}, {"true", TokenType::TRUE},
                                                {"var", TokenType::VAR}, {"while", TokenType::WHILE}};

}  // namespace cpplox