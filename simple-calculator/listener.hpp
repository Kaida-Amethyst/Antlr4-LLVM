#include "antlr4-runtime.h"
#include "calcBaseListener.h"
#include "calcLexer.h"
#include "calcParser.h"
#include <format>
#include <iostream>
#include <string_view>

class ProgramListener : public calcBaseListener {
private:
  std::set<std::string> variables;
  std::string_view code;

public:
  ProgramListener(std::string_view code) : calcBaseListener(), code(code) {}

  virtual void enterWith_stmt(calcParser::With_stmtContext *ctx) override {
    auto vars = ctx->Var();
    auto contains = [&](std::string &n) -> bool {
      return this->variables.find(n) != this->variables.end();
    };
    // check duplicate variables
    for (auto var : vars) {
      auto name = var->getText();
      if (contains(name)) {
        int pos = var->getSymbol()->getStartIndex();
        std::cout << this->code << std::endl;
        std::cout << std::string(pos, ' ') << '^';
        std::cout << "Error: duplicate variable name: " << name << std::endl;
        exit(1);
      }
      this->variables.insert(name);
    }
  }

  virtual void enterPrimary(calcParser::PrimaryContext *ctx) override {
    if (ctx->Var()) {
      auto name = ctx->Var()->getText();
      if (this->variables.find(name) == this->variables.end()) {
        int pos = ctx->Var()->getSymbol()->getStartIndex();
        std::cout << this->code << std::endl;
        std::cout << std::string(pos, ' ') << '^';
        std::cout << "Error: variable not defined: " << name << std::endl;
        exit(1);
      }
    }
  }
};
