#include <iostream>
#include <string>
#include "lox.h"
#include "token.h"


auto main(int argc, const char *argv[]) -> int {
  cpplox::Lox driver;
  if (argc > 1) {
    std::cout << "Usage: cpplox [script]\n";
    return 0;
  } 
  if (argc == 1) {
    driver.RunFile(argv[0]);
  } else {
    driver.RunPrompt();  
  }
  return 0;
}