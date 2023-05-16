# Chapter 1 - Installation

---

# Prerquirement

首先需要确保`gcc`​，`g++`​,`git`​，`cmake`​，`ninja-build`​都已经安装，这是最基础的5个工具。一般而言，在比较好的arch linux发行版上可能已经安装好。如果是在ubuntu下，可使用命令：

```shell
$ sudo apt install -y gcc g++ git cmake ninja-build
```

建议直接在ubuntu20.04下安装，这样`gcc`​和`g++`​直接就是9.3.0的版本，`cmake`​可能需要尝试使用如下的命令：

```shell
$ sudo snap install cmake
```

如果已经有`cmake`​，只是版本不高的话（譬如cmake版本只有3.5），那么可以直接下载高版本的cmake：[https://cmake.org/files/](https://cmake.org/files/)，在`/usr/bin`​目录下建立软链接。如果本身没有`cmake`​的话，最好还是使用`apt`​，否则没法下载安装依赖库。

# Antlr4 Installation

antlr4本身是java写的，因此首先需要下载安装java。

首先运行`javac --version`​，先看一下是否已经下载安装好了java，如果没有的话，需要下载安装java。

## Java下载

对于适当的linux发行版来说，可以直接使用命令，注意java分成两个版本：JRE和JDK，如果只需要运行java程序，下载JRE，如果还希望做java开发，下载JDK。如果不确定，就无脑下载JDK。

* Debian系（Ubuntu，deepin等）：`sudo apt-get install default-jre`​或`sudo apt-get install default-jdk`​
* Arch系：`sudo pacman -S jre-openjdk`​或者`sudo pacman -S jdk-openjdk`​

如果以上的命令行不通，可以直接从官网上下载安装最新版。

* JRE下载：https://www.java.com/en/download/
* JDK下载：https://www.oracle.com/java/technologies/jdk-script-friendly-urls/

将下载下来的压缩包解压，放在特定位置下。然后在`/usr/bin`​目录下做`java`​和`javac`​的软链接。确认在命令行下使用`java -version`​或者`javac -version`​有正确的输出即可。

## Antlr下载

可以直接在ANTLR官网下载jar包：

https://www.antlr.org/download.html

不过官网下载基本是最新版本，有时会出现所需要的JVM版本过高的情况，此时可以选择旧版本，在github上下载：

https://github.com/antlr/website-antlr4/tree/gh-pages/download

## ANTLR安装

将下载下来的jar包推荐放在`/usr/local/lib`​下，然后在`.bashrc`​或者`.bash_profile`​下设定CLASSPATH（假设下载的是4.12的jar包）：

```shell
export CLASSPATH=".:/usr/local/lib/antlr-4.12-complete.jar:$CLASSPATH"
```

然后就可以通过java来启动ANTLR，有两种方式：

```shell
$ java -jar /usr/local/lib/antlr-4.12-complete.jar
```

或者：

```shell
$ java org.antlr.v4.Tool
```

一般而言会出现如下的信息：

```plaintext
ANTLR Parser Generator  Version 4.10
 -o ___              specify output directory where all output is generated
 -lib ___            specify location of grammars, tokens files
 -atn                generate rule augmented transition network diagrams
 -encoding ___       specify grammar file encoding; e.g., euc-jp
 -message-format ___ specify output style for messages in antlr, gnu, vs2005
 -long-messages      show exception details when available for errors and warnings
 -listener           generate parse tree listener (default)
 -no-listener        don't generate parse tree listener
 -visitor            generate parse tree visitor
 -no-visitor         don't generate parse tree visitor (default)
 -package ___        specify a package/namespace for the generated code
 -depend             generate file dependencies
 -D<option>=value    set/override a grammar-level option
 -Werror             treat warnings as errors
 -XdbgST             launch StringTemplate visualizer on generated code
 -XdbgSTWait         wait for STViz to close before continuing
 -Xforce-atn         use the ATN simulator for all predictions
 -Xlog               dump lots of logging info to antlr-timestamp.log
```

每次都要输入这么一长串命令显然很不方便，最好设定一个别名，在`.bashrc`​和`.bash_profile`​下设定：

```shell
$ alias antlr4='java -jar /usr/local/lib/antlr-4.12-complete.jar'
```

这样之后就可以直接使用`antlr4`​命令来启动antlr了。

另外，由于我们主要是在C++下进行开发，因此可以添加一个新的别名缩写：

```shell
$ alias antlr4cpp='java -jar /usr/local/lib/antlr-4.12-complete.jar -Dlanguage=Cpp'
```

这样，之后使用`antlr4cpp`​就可以直接生成C++的代码文件。

## Antlr4 cpp runtime

上面的antlr4是一个代码生成工具，它的作用是把语法文件生成我们所希望的用特定语言写成的词法分析器和语法分析器。但是所生成的词法和语法分析器仍然需要antlr4的cpp runtime工具来支持。简单地说，就是所生成的词法和语法分析工具引入了一系列的库和头文件，需要单独安装。

到antlr4的官网下载antlr4-cpp runtime：https://www.antlr.org/download.html，注意这个runtime需要单独编译。确保你的机器上可以使用`g++`​和`cmake`​命令。

```shell
$ ls
antlr4-cpp-runtime-4.12.0-source
$ mv antlr4-cpp-runtime-4.12.0-source antlr4-source-code
$ cd antlr4-source-code
antlr4-source-code$ ls
cmake  CMakeLists.txt  demo  deploy-macos.sh  deploy-source.sh  deploy-windows.cmd  dist  LICENSE.txt  README.md  runtime  VERSION
antlr4-source-code$ 
```

注意这里有一个`CMakeLists.txt`​。我们使用cmake来构建即可。

```shell
antlr4-source-code$ cmake -B build && cmake --build build
```

或者：

```shell
antlr4-source-code$ mkdir build && cd build && cmake .. && make -j6
```

最后使用`sudo make install`​即可把库和库文件安装到`/usr/local/`​下了。

```shell
antlr4-source-code$ sudo make insall
```

安装好后，在`/usr/local/`​下，可以看到`antlr`​的runtime库，主要是`/usr/local/include/antlr4-runtime`​和`/usr/local/lib/`​下的antlr4的静态库和动态库。

最后需要额外记住的是，在^（如果使用其它shell，如zsh或者fish，那么需要添加到对应的shell 配置文件里去。）^​``​里面^（如果使用其它shell，如zsh或者fish，那么需要添加到对应的shell 配置文件里去。）^，务必要将它们加入到`include`​的`library`​的路径里面。

```bash
export CPLUS_INCLUDE_PATH=/usr/local/include/:$CPLUS_INCLUDE_PATH
export CPLUS_INCLUDE_PATH=/usr/local/include/antlr4-runtime:$CPLUS_INCLUDE_PATH
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

然后，在写C++的时候，就可以通过`#include "antlr4-runtime.h"`​来使用`antlr4`​的runtime库了。

这里要注意一下，不能仅仅写`export CPLUS_INCLUDE_PATH=/usr/local/include/:$CPLUS_INCLUDE_PATH`​，因为`antlr4`​生成的那些词法与语法文件都是`#include "antlr4-runtime.h`​。

# LLVM安装

LLVM原版在github上，如果打不开github，gitee上也有镜像库。

* github库位置：[https://github.com/llvm/llvm-project.git](https://github.com/llvm/llvm-project.git)
* gitee镜像位置：[https://gitee.com/mirrors/LLVM.git](https://github.com/llvm/llvm-project.git)

## 下载LLVM

整个LLVM可能有几个G，全部下载下来可能有点麻烦。不过，这么大并不是因为LLVM本身很大，而是因为LLVM发展了很多年，所积累下来的git记录有很多。其实可以不需要这些记录，只下载最新的版本。可以在llvm的官网上先查到最新版本的信息，例如在笔记写成是llvm的最新版本是`16.0.0`​，那么就使用命令：

```
git clone --depth  1 https://github.com/llvm/llvm-project.git -b llvmorg-16.0.0
```

## 构建

在`llvm-project`​创建`build`​目录，然后在`build`​目录下，运行如下命令：

```
cmake -G Ninja -DLLVM_ENABLE_PROJECTS=clang \
               -DLLVM_ENABLE_RTTI=ON        \
               -DCMAKE_BUILD_TYPE="Release" \
               ../llvm
```

稍微注意一下这里的`-DLLVM_ENABLE_PROJECTS`​，这里是llvm提供的一些C和C++的一些编程工具，我这里顺便安装了clang，也可以选择其它的工具，可以在下载下来的`LLVM`​下的`llvm`​目录里的`CMakeLists.txt`​中，直接看到所有的工具：

```cmake
set(LLVM_ALL_PROJECTS "bolt;clang;clang-tools-extra;compiler-rt;cross-project-tests;libc;libclc;lld;lldb;mlir;openmp;polly;pstl")
```

如果想顺便安装`clang`​和`lldb`​，那么命令中这样写就好：

```cmake
cmake -G Ninja -DLLVM_ENABLE_PROJECTS="clang;lldb" \
               -DLLVM_ENABLE_RTTI=ON        \
               -DCMAKE_BUILD_TYPE="Release" \
               ../llvm
```

当然也可以不怕麻烦，直接使用`-DLLVM_ENABLE_PROJECTS=all`​，构建所有工具，但是这样可能会比较占空间。

另外需要注意的是`-DLLVM_ENABLE_RTTI=ON`​，必须要添加这一个选项，因为antlr4大量使用C++的RTTI，但是LLVM是默认关闭RTTI的，因此在构建的过程中就需要把RTTI开启。

使用cmake构建完后，运行`ninja`​进行构建：

```
ninja
```

如果使用上面的命令，可能电脑会变卡，因为这个命令是使用计算机处理器上所有的core。如果不想使用所有core，可以使用`-jx`​（`x`​写上具体的数字），来指定使用多少个核：

```
ninja -j2
```

在构建完成后，可以检测一下是否构建成功：

```
ninja check-all
```

上一步可能需要相当的时间，一个比较简单的办法就是直接看`llc`​的版本，在`build`​下运行

```
./bin/llc -version
```

最后，进行安装：

```
sudo ninja install
```

会安装到`/usr/local`​目录下，之前我们已经把`/usr/local`​加入到C++的头文件搜索路径里了，这里就不做额外的操作了。

‍
