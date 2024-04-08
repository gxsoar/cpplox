#include "lox.h"
#include <error.h>
#include <cstdlib>
#include <memory>
#include "parser.h"

namespace cpplox {

auto Lox::RunFile(const std::string &filePath) -> void {
  std::ifstream file{filePath};
  if (file.bad()) {
    throw std::runtime_error("Cannot open file\n");
  }
  std::ostringstream str;
  str << file.rdbuf();
  auto source_str = str.str();
  Run(source_str);
  if (had_error) {
    exit(-1);
  }
  if (had_runtime_error) {
    exit(70);
  }
}

auto Lox::RunPrompt() -> void {
  std::cout << "Cpplox\n";
  std::string line;
  for (;;) {
    std::cout << "> ";
    line.clear();
    const auto &read_val = std::getline(std::cin, line);
    if (read_val.eof() || read_val.bad() || line == "q") {
      break;
    }
    Run(line);
  }
}

auto Lox::Run(const std::string &source) -> void {
  auto scanner = std::make_unique<cpplox::Scanner>(source);
  auto tokens = scanner->ScanTokens();

  auto parser{std::make_unique<Parser>(tokens)};
  auto expression {parser->Parse()};
  if (had_error) {
    return;
  }
  interpreter->Interpret(expression);
}


}  // namespace cpplox