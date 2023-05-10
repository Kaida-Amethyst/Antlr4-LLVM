#include "antlr4-runtime.h"
#include "calcBaseVisitor.h"
#include "calcLexer.h"
#include "calcParser.h"
#include <format>
#include <iostream>
#include <string_view>

// #include <llvm/IR/LLVMContext.h>
// #include <llvm/IR/Module.h>

// using namespace llvm;

class ProgramVisitor : public calcBaseVisitor {

public:
  // llvm::LLVMContext context;
  // llvm::Module *calc_mod;
  // Type *int_type;
  // Function *calc_func;
  // BasicBlock *entry_block;
  // IRBuilder<> *builder;
  // std::map<std::string, Value *> symbol_table;

  virtual std::any visitProgram(calcParser::ProgramContext *ctx) override {
    // calc_mod = new Module("calc", context);
    // int_type = Type::getInt32Ty(context);
    return visitChildren(ctx);
  }

  virtual std::any visitProg(calcParser::ProgContext *ctx) override {
    // if it has with statement
    // ctx->with_stmt() is not null
    if (nullptr == ctx->with_stmt()) {
      // calc_func = Function::Create(FunctionType::get(int_type, {}, false),
    } else {
      // get number of variables from with statement
      // std::any num_of_vars = ctx->with_stmt()->Var().size();
      // int n = std::any_cast<int>(num_of_vars);
      // if (n == 0) throw error
      // auto arg_types = std::vector<Type *>(n, int_type);
      // calc_func = Function::Create(FunctionType::get(int_type, arg_types,
      // false), Function::ExternalLinkage, "calc", calc_mod);
      // visitWith_stmt(ctx->with_stmt());
      // entry_block = BasicBlock::Create(context, "entry", calc_func);
      // Then visit expr
      // visitExpr(ctx->expr());
    }
    return visitChildren(ctx);
  }

  virtual std::any visitWith_stmt(calcParser::With_stmtContext *ctx) override {
    // if (ctx->Var().size() == 0) throw error
    // auto arg_iter = calc_func->arg_begin();
    // for (auto var : ctx->Var()) {
    //   std::string varname = var->getText();
    //   std::cout << varname << std::endl;
    //   // if (symbol_table.find(varname) != symbol_table.end()) { // varname
    //   is
    //   // not unique
    //   //   // throw error
    //   // }
    //   // // Insert a value into the symbol table. the value is a parameter of
    //   // the calc function symbol_table[varname] = arg_iter++;
    // }
    // return ctx->Var().size();
    return visitChildren(ctx);
  }

  virtual std::any visitExpr(calcParser::ExprContext *ctx) override {
    return visitChildren(ctx);
  }
};

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
  ProgramVisitor visitor;
  std::any result = visitor.visit(tree);

  return 0;
}
