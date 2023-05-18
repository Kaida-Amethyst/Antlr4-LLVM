# 使用Antlr4与LLVM构建自己的编译器（非正式版）

----------------------------------

本教程旨在展示如何利用现代工具：C++20，CMake，Antlr4和LLVM来构建一个简易的编译系统。

## 谁更适合阅读

本教程假设读者只有基础的C++知识，也就是能够使用C++做出一些leetcode medium级别的题目，并不要求读者需要这些工具的前置知识。特别地，对于编译原理，并不要求读者上过编译原理的课程。

## 前期知识

尽管本教程使用了大量的现代工具，但读者仍然需要具备一定的基础才能保证看懂本篇教程，以下是本教程必备的前置知识：

- 基础的C编程，知道C语言的编译流程，即预处理，编译，汇编和链接四个阶段，
- C++基础，包括变量，函数，类，枚举等的定义。

为了更好地理解本教程，读者如果有以下的知识会更有助于教程的理解：

- 编译原理
- C++11编程
- Makefile

## 环境要求

- 操作系统：建议使用ubuntu 20.04以上的版本。
- 编译环境：支持C++20即可。
- Antlr4 4.12
- LLVM 12以上的版本
- CMake 3.20

## Blogs

[Chapter-1-Installation](./Blogs/Chapter-1-Installation.md)

[Chapter-2-Basic-Workflow-Part-I](./Blogs/Chapter-2-Basic-Workflow-Part-I.md)

## Roadmap

- [x] 环境搭建
- [ ] 编译原理快速入门
- [x] 超级简单的calc语言，Basic Workflow Overview
- [ ] 超级简单的calc语言，Basic Workflow full view
- [ ] 超级简单的calc语言，Antlr4，词法分析与语法分析
- [ ] 超级简单的calc语言，CMake，搭建构建系统
- [ ] 超级简单的calc语言，LLVM，生成后端代码
- [ ] 稍微复杂一点的语言(没想好名称)，Antlr4, Part I
- [ ] 稍微复杂一点的语言，Antlr4, Part II
- [ ] 稍微复杂一点的语言，Antlr4, Part III
- [ ] 稍微复杂一点的语言，CMake
- [ ] 稍微复杂一点的语言，LLVM，Part I
- [ ] 稍微复杂一点的语言，LLVM，Part II
- [ ] 稍微复杂一点的语言，LLVM，Part III
- [ ] 稍微复杂一点的语言，LLVM，Part IV


