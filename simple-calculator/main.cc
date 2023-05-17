#include "antlr4-runtime.h"
#include "calcBaseVisitor.h"
#include "calcLexer.h"
#include "calcParser.h"
#include <format>
#include <iostream>
#include <string_view>

#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

using llvm::BasicBlock;
using llvm::Function;
using llvm::FunctionType;
using llvm::IRBuilder;
using llvm::LLVMContext;
using llvm::Module;
using llvm::Type;
using llvm::Value;

#include "listener.hpp"

class ProgramVisitor : public calcBaseVisitor {

public:
  LLVMContext context;
  Module *calc_mod;
  Type *int_type;
  FunctionType *calc_func_type;
  Function *calc_func;
  BasicBlock *entry_block;
  IRBuilder<> *builder;
  std::map<std::string, Value *> symbol_table;

  ProgramVisitor() {
    calc_mod = new Module("calc", context);
    int_type = Type::getInt32Ty(context);
    calc_func_type = nullptr;
    calc_func = nullptr;
    builder = new IRBuilder<>(context);
    entry_block = nullptr;
  }

  ~ProgramVisitor() {
    delete calc_mod;
    delete builder;
  }

  // llvm::Module compile() {}

  void printIR() { calc_mod->print(llvm::outs(), nullptr); }

  virtual std::any visitProgram(calcParser::ProgramContext *ctx) override {
    // then create main function
    // return calc_mod;
    //   return visitChildren(ctx);
    return visitProg(ctx->prog());
  }

  virtual std::any visitProg(calcParser::ProgContext *ctx) override {
    if (!ctx->with_stmt()) {
      calc_func_type = FunctionType::get(int_type, {}, false);
    } else {
      int n = ctx->with_stmt()->Var().size();
      // if (0 == n) error, but suppose it's not 0 temperaily
      auto func_args = std::vector<Type *>(n, int_type);
      calc_func_type = FunctionType::get(int_type, func_args, false);
    }
    calc_func = Function::Create(calc_func_type, Function::ExternalLinkage,
                                 "calc", calc_mod);
    visitWith_stmt(ctx->with_stmt());
    entry_block = BasicBlock::Create(context, "entry", calc_func);
    builder->SetInsertPoint(entry_block);
    auto r = visitExpr(ctx->expr());
    Value *ret = std::any_cast<Value *>(r);
    builder->CreateRet(ret);

    return nullptr;
  }

  virtual std::any visitWith_stmt(calcParser::With_stmtContext *ctx) override {
    if (ctx == nullptr)
      return nullptr;
    // if (ctx->Var().size() == 0) throw error
    auto arg_iter = calc_func->arg_begin();
    for (auto var : ctx->Var()) {
      std::string varname = var->getText();
      // if (symbol_table.find(varname) != symbol_table.end()) { // varname
      //   std::cerr << "var name is not unique" << std::endl;
      // }
      symbol_table[varname] = arg_iter++;
    }
    return nullptr;
  }

  virtual std::any visitExpr(calcParser::ExprContext *ctx) override {
    Value *res = nullptr;
    if (ctx->expr()) {
      Value *l = std::any_cast<Value *>(visitExpr(ctx->expr()));
      Value *r = std::any_cast<Value *>(visitMul(ctx->mul()));
      if (ctx->op->getText() == "+") {
        res = builder->CreateAdd(l, r);
      } else if (ctx->op->getText() == "-") {
        res = builder->CreateSub(l, r);
      }
    } else {
      res = std::any_cast<Value *>(visitMul(ctx->mul()));
    }
    return res;
  }

  virtual std::any visitMul(calcParser::MulContext *ctx) override {
    Value *res = nullptr;
    if (ctx->mul()) {
      Value *l = std::any_cast<Value *>(visitMul(ctx->mul()));
      Value *r = std::any_cast<Value *>(visitPrimary(ctx->primary()));
      if (ctx->op->getText() == "*") {
        res = builder->CreateMul(l, r);
      } else if (ctx->op->getText() == "/") {
        res = builder->CreateSDiv(l, r);
      }
    } else {
      res = std::any_cast<Value *>(visitPrimary(ctx->primary()));
    }
    return res;
  }

  virtual std::any visitPrimary(calcParser::PrimaryContext *ctx) override {
    if (ctx->Num()) {
      int n = std::stoi(ctx->Num()->getText());
      Value *v = builder->getInt32(n);
      return v;
    } else if (ctx->Var()) {
      return symbol_table.at(ctx->Var()->getText());
    }
    return visitExpr(ctx->expr());
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
  // for (auto token : tokens.getTokens()) {
  //   std::cout << token->toString() << std::endl;
  // }

  // Create parser
  calcParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.program();

  // to string
  std::cout << tree->toStringTree(&parser) << std::endl;

  // Create listener
  ProgramListener listener(source_code);
  antlr4::tree::ParseTreeWalker::DEFAULT.walk(&listener, tree);

  // Create visitor
  ProgramVisitor visitor;
  std::any result = visitor.visit(tree);

  visitor.printIR();

  return 0;
}
