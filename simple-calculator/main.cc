#include "antlr4-runtime.h"
#include "calcBaseVisitor.h"
#include "calcLexer.h"
#include "calcParser.h"
#include <format>
#include <iostream>
#include <string_view>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Must provide source code!" << std::endl;
    exit(1);
  }

  std::string_view source_code = argv[1];

  antlr4::ANTLRInputStream input(source_code);

  // Create lexer
  calcLexer lexer(&input);
  antlr4::CommonTokenStream tokens(&lexer);

  tokens.fill();

  // Print tokens
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }

  // Create parser
  calcParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.program();

  // to string
  std::cout << tree->toStringTree(&parser) << std::endl;

  // Create visitor
  // tryVisitor visitor;
  // std::any result = visitor.visit(tree);
  //
  // std::cout << std::format("Result: {}", std::any_cast<int>(result))
  //           << std::endl;

  return 0;
}
