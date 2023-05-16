# Chapter 2 - Basic Workflow Part I

---

# Object

接下来的几章首先来练习全部的流程。注意这一章作为联系，我们来熟悉整个的workflow，先暂时不要使用自动化的构建工具和测试工具，并且先假设我们输入的代码不会出错。整个workflow如下：

* 首先，利用Antlr4来完成对一个超级简单的表达式语言的词法和语法分析，其形式类似于：

  ​`with a : a + 1;`​

  ​`with a, b : 3 * (a + b);`​

  ​`3+5*4;`​

  这个表达式语言，要么是一个单纯的数字表达式，要么是一个以with开头，声明一些变量，而后是一个带有变量的表达式。
* 接着在词法分析和语法分析的过程中，生成llvm IR。
* 将llvm IR编译成x86汇编，再生成可重定位目标文件。
* 编写main函数来调用所生成的函数，生成可执行文件。

# Calc

新建一个`calc`​目录，在这个目录下新建一个`calc.g4`​文件，这个是`antlr4`​支持的语法文件。

​`calc.g4`​的信息如下，以小写字母为开头的在antlr4里面默认是非终结符，大写字母开头的默认为终结符。它可以识别类似`with a, b: a *(1 + b);`​之类的语句。

```g4
grammar calc;

program: prog;

prog: (with_stmt ':')? expr ';' ;

with_stmt : 'with' Var (',' Var )* ;

expr : expr ( op=('+' | '-') mul )
     | mul
     ;

mul  : mul ( op=('*' | '/') primary )
     | primary
     ;

primary : Var
        | Num
        | '(' expr ')'
        ;

Num : [0-9]+;

Var : [a-zA-Z]+;

WS  : [ \t\r\n]+ -> skip;
```

# Generate Cpp Code

有了g4文件之后，接下来可以使用`antlr4`​工具来生成代码。使用如下的命令：

```shell
$ antlr4 -Dlanguage=Cpp calc.g4 -visitor
```

注意这里我们使用了`-visitor`​的选项。

之后，便可以在`calc`​目录下，观察到数个`.cpp`​，`.h`​和其它文件。

```shell
calc$ ls
calc.tokens           calcLexer.interp     calcParser.cpp  calcVisitor.cpp   calc.g4
calcBaseVisitor.cpp   calcLexer.cpp        calcParser.h    calcVisitor.h 
calc.interp           calcBaseVisitor.h    calcLexer.h     calcLexer.tokens
```

我们这里暂时不对这些文件作修改，先直接写我们的`main.cc`​文件，这里我们特意使用了`.cc`​而不是`.cpp`​。

在main.cc下，我们可以先尝试把词法和语法分析的结果先打印出来，看一下是否符合我们的预期。

```cpp
#include "antlr4-runtime.h"
#include "calcLexer.h"
#include "calcParser.h"
#include <format>
#include <iostream>
#include <string_view>

int main(int argc, char *argv[]) {
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

  return 0;
}
```

在这个main函数中，我们首先从标准输入中获取代码，然后，将代码传入`ANTLRInputStream`​中，接着调用Lexer，打印词法分析的结果，再调用Parser。接着构造一个语法树，再把这个语法树打印出来。稍微注意一下这一行代码：

```cpp
antlr4::tree::ParseTree *tree = parser.program();
```

这里调用了一个`program`​，这个是根据先前的`.g4`​文件来的，因为`.g4`​文件中，语法树的根节点是`program`​。

我们尝试来编译所有的文件，使用如下的命令：

```shell
$ g++ -o calc *.cpp main.cc -lantlr4-runtime -std=c++17 -fexception
```

之后，运行`calc`​程序，看一下是否能够解析`with a, b: a*b;`​这样的代码。

```shell
$ ./calc "with a, b: a*b;"  
[@0,0:3='with',<3>,1:0]
[@1,5:5='a',<12>,1:5]
[@2,6:6=',',<4>,1:6]
[@3,8:8='b',<12>,1:8]
[@4,9:9=':',<1>,1:9]
[@5,11:11='a',<12>,1:11]
[@6,12:12='*',<7>,1:12]
[@7,13:13='b',<12>,1:13]
[@8,14:14=';',<2>,1:14]
[@9,15:14='<EOF>',<-1>,1:15]
(program (prog (with_stmt with a , b) : (expr (mul (mul (primary a)) * (primary b))) ;))
```

输出结果的最后一行就是语法树的lisp形式的结构，可以看出符合我们的预期。

# LLVM

然后，我们来为我们的代码生成llvm IR，我们先来写一个`test_calc.c`​文件，里面放置一个`main`​函数，并且声明一个`calc`​函数：

```cpp
#include <stdio.h>

int calc(int, int, int);

int main() {
  int r = calc(10, 20, 30);
  printf("r = %d\n", r);
  return 0;
}
```

但是我们不要直接去实现这里的`calc`​函数，这个calc函数通过我们的`calc`​语言去生成。我们假定这里是`with a, b, c: (a+b+c)*3;`​。

然后，我们来实现我们的llvm的代码：

首先引入必要的头文件：

```cpp
// main.cc
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

#include "calcBaseVisitor.h"
```

然后，我们需要一个工具来帮助我们遍历语法树，antlr4已经为我们生成一个名为`calcBaseVisitor`​的类了，来看一下现有的`calcBaseVisitor.h`​的内容：

```cpp
// Generated from calc.g4 by ANTLR 4.12.0
#pragma once
#include "antlr4-runtime.h"
#include "calcVisitor.h"

/**
 * This class provides an empty implementation of calcVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  calcBaseVisitor : public calcVisitor {
public:

  virtual std::any visitProgram(calcParser::ProgramContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProg(calcParser::ProgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWith_stmt(calcParser::With_stmtContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpr(calcParser::ExprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMul(calcParser::MulContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimary(calcParser::PrimaryContext *ctx) override {
    return visitChildren(ctx);
  }
};
```

这个类其实可以直接使用，在main函数里面使用：

```cpp
  calcBaseVisitor visitor;
  auto result = visitor.visit(tree);
```

但是因为没有修改这个类，因此可能看不出到底发生了什么，读者可以尝试在每一个函数里面进行print来观察一下运行的轨迹。

正常来说，我们可以直接在这个类上进行修改来达成我们的目的，这里为了简便，我们在`main.cc`​下，用一个新类来继承`calcBaseVisitor`​。

```cpp
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
};
```

写好这个类的构造函数和析构函数后，接下来完成剩余部分。

首先是根节点，根节点的遍历函数就是`visitProgram`​，原本的`visitChildren`​现在修改为`visitProg`​，根据语法结构，实际上没有变化，因为`program`​的子节点本来就只有`prog`​。

```cpp
virtual std::any visitProgram(calcParser::ProgramContext *ctx) override {
  return visitProg(ctx->prog());
}
```

但是在`prog`​下，需要进行一些修改。按照我们的约定，我们的calc语言可以有带`with`​和不带`with`​的两种，通过`ctx->with_stmt()`​的返回是否为空可以知道我们输入的代码是带with的还是不带with的。如果是带with的，那么我们生成的`calc`​函数需要带参数，不带`with`​的话就不需要参数：

```cpp
virtual std::any visitProg(calcParser::ProgContext *ctx) override {
  if (!ctx->with_stmt()) {
    calc_func_type = FunctionType::get(int_type, {}, false);
  } else {
    int n = ctx->with_stmt()->Var().size();
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
```

上面代码中的if语句根据带参和不带参两种情况决定calc的函数类型，之后通过`Function::Create`​就是创建了一个名为calc的函数。紧接着调用`visitWith_stmt`​函数来对参数进行处理，主要是将参数加入到符号表里面。接着创建一个入口块(entry_block)，再通过遍历`Expr`​来插入语句，最后将遍历`Expr`​的结果作为整个函数的返回值。

来看一下`visitWith_stmt`​函数，主要作用是将函数的参数加入到符号表内：

```cpp
  virtual std::any visitWith_stmt(calcParser::With_stmtContext *ctx) override {
    if (ctx == nullptr)
      return nullptr;

    auto arg_iter = calc_func->arg_begin();
    for (auto var : ctx->Var()) {
      std::string varname = var->getText();
      symbol_table[varname] = arg_iter++;
    }
    return nullptr;
  }
```

然后是`visitExpr`​函数，这里额外提醒注意`.g4`​文件的写法：

```g4
expr : expr ( op=('+' | '-') mul )
     | mul
     ;
```

expr的结构有两种可能，一种是带有`expr`​的子节点，也就是第一种情况，另一种是不带`expr`​的子节点，也就是第二种情况。根据这两种情况的不同，来做不同的处理：

```cpp
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
```

如果有`expr`​子节点，那么就先遍历左子树，再遍历右子树，最后根据`op`​（注意这个op是根据`.g4`​文件里来的）来判断是生成add指令还是sub指令。

然后是`visitMul`​，基本的思路与`visitExpr`​一致：

```cpp
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
```

最后是`visitPrimary`​，根据`Var()`​，`Num()`​和`expr()`​是否为空，采取不同的动作：

```cpp
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
```

在遍历完成后，我们的llvm ir也建立好了，接下来就是想办法将ir进行输出，在`ProgramVisitor`​类当中添加一个`printIR`​函数。

```cpp
void printIR() {
  calc_mod->print(llvm::outs(), nullptr);
}
```

之后，在main函数中，调用这个printIR即可，这里贴出全代码：

```cpp
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

  // Create parser
  calcParser parser(&tokens);
  antlr4::tree::ParseTree *tree = parser.program();


  // Create visitor
  ProgramVisitor visitor;
  std::any result = visitor.visit(tree);

  visitor.printIR();

  return 0;
}
```

之后进行编译，llvm的编译选项比较多，这里使用`llvm-config`​命令来进行辅助编译：

```cpp
g++ -o calc *.cpp main.cc    \
            -std=c++17       \ 
            -lantlr4-runtime \
            `llvm-config --cxxflags --ldflags --system-libs --libs core` \
            -fexceptions
```

之后，使用`./calc "with a, b, c: (a+b+c)*3;"`​命令即可生成对应llvm IR。

```shell
$ ./calc `"with a, b, c: (a+b+c)*3;"
; ModuleID = 'calc'
source_filename = "calc"

define i32 @calc(i32 %0, i32 %1, i32 %2) {
entry:
  %3 = add i32 %0, %1
  %4 = add i32 %3, %2
  %5 = mul i32 %4, 3
  ret i32 %5
}
```

我们把生成的代码输出到calc.ll文件中，接着使用`llc`​命令来编译成x86汇编：

```shell
$ ./calc `"with a, b, c: (a+b+c)*3;" > calc.ll
$ llc -march=x86-64 -filetype=asm calc.ll
```

之后，就可以看到`calc.s`​文件，这里可以看一下这个`calc.s`​文件：

```x86asm
	.text
	.file	"calc"
	.globl	calc                            # -- Begin function calc
	.p2align	4, 0x90
	.type	calc,@function
calc:                                   # @calc
	.cfi_startproc
# %bb.0:                                # %entry
                                        # kill: def $edi killed $edi def $rdi
	addl	%esi, %edi
	addl	%edx, %edi
	leal	(%rdi,%rdi,2), %eax
	retq
.Lfunc_end0:
	.size	calc, .Lfunc_end0-calc
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
```

可以看到这个`calc`​文件里已经有了一个calc的函数。

接着，将我们的`calc.s`​和`test_calc.c`​一起进行编译，然后运行这个test可以看到：

```shell
$ gcc -o test calc.s test_calc.c
$ ./test
r = 180
```

至此，我们的第一次antlr4和llvm的联合就成功了。

但是这个workflow仍然有很多的问题，例如，我们没有检测可能出现的错误，我们也没有为其编写自动化工具例如Makefile或者CMake。在下一节中，我们将整个workflow再次进行梳理，编写自动化构建工具和测试工具，添加差错功能。之后，我们再来实现一个更加完整的，图灵完备的语言。
