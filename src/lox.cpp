#include "lox.h"

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
  if (had_error_) {
    exit(-1);
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

  for (auto &token : tokens) {
    // Todo: output token
  }
}

auto Lox::Report(int line, const std::string &where, const std::string &message) -> void {
  std::cerr << "[line " << line << "] Error " << where << " : " << message << "\n";
  had_error_ = true;
}

}  // namespace cpplox