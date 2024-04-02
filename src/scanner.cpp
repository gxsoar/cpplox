#include "scanner.h"
#include <list>
#include "token.h"

namespace cpplox {

auto Scanner::ScanTokens() -> std::list<Token> {
  while(!IsAtEnd()) {

  }

  return tokens_;
}

}  // namespace cpplox