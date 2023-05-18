#include "antlr4-runtime.h"
#include "calcBaseVisitor.h"
#include "calcLexer.h"
#include "calcParser.h"
#include <format>

#include "listener.hpp"
#include "visitor.hpp"


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
  // for (auto token : tokens.getTokens()) {
  //   std::cout << token->toString() << std::endl;
  // }

  // Create parser
  calcParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.program();

  int noferr = parser.getNumberOfSyntaxErrors();
  if (noferr > 0) {
    std::cerr << "Found " << noferr << " syntax errors." << std::endl;
    exit(1);
  }

  // check syntax error
  if (parser.getNumberOfSyntaxErrors() > 0) {
    std::cerr << "Syntax error!" << std::endl;
    exit(1);
  }

  // to string
  std::cout << tree->toStringTree(&parser) << std::endl;

  // Create listener
  ProgramListener listener(source_code);
  antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  // Create visitor
  // ProgramVisitor visitor;
  // std::any result = visitor.visit(tree);
  //
  // visitor.printIR();

  return 0;
}
