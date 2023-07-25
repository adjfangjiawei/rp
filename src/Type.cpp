#include "errors.h"
#include "printer.h"
#include <exception>
#include <iostream>
#include <pprint.hpp>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>

// 抽象类
template <class Print> class ExprAST {
public:
  virtual void pprint() = 0;

private:
};

// 变量语法树
template <class Print> class VariableExprAST : public ExprAST<Print> {
public:
  virtual void pprint() = 0;
};

// 立即数语法树
template <class Print> class InstantValueExprAST : public ExprAST<Print> {
  virtual void pprint() = 0;
};

// 一元表达式
template <class Print> class UnaryExprAST : public ExprAST<Print> {
public:
private:
  char op;
  ExprAST<Print> *expr;
};

// 整型变量
template <class Print>
class IntegerVariableExprAST : public VariableExprAST<Print> {
public:
  virtual void pprint() {}

private:
  int val;
  std::string name;
};

// 整型立即数
template <class Print>
class IntegerInstantValueExprAST : public InstantValueExprAST<Print> {
public:
  IntegerInstantValueExprAST(int value, Print *printer)
      : printer(printer), val(value) {}
  virtual void pprint() {}

private:
  int val;
  Print *printer;
};

auto get_Token(std::string &source, int index) {
  while (source[index] != ' ' && source[index] != '\n' &&
         source[index] != '\t') {
    index++;
  }
}

// 浮点型立即数
int main() {

  std::error_code error = mylib::errc::isLegalVariableName;
  std::string hello = "njwdnek";
  auto &&[printer, sprinter] = getPPrint();
  if (error.value() != 0) {
    // printer.print({1, 2});
    printer.print(std::make_tuple(1, "jdis"));
    printer.print(mylib::errc::isLegalVariableName);
    printer.print((mylib::errc::my_error)error.value());
    spdlog::error("fatal error");
    spdlog::error("错误码为{},{}", (mylib::errc::my_error)error.value(), hello);
  }
  spdlog::info("Entering main");
  int a = 100;

  printer.print(5);

  std::stringstream stream;
  pprint::PrettyPrinter printer1(stream);
  printer1.print(std::make_tuple(1, "jdis"));
  std::string atxc;

  // std::cout << stream.str() << std::endl;
  stream.str("");
  printer1.print(std::make_tuple(1, "jdis"));
  std::cout << stream.str() << std::endl;
}