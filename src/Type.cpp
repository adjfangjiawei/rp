#include "errors.h"
#include <exception>
#include <iostream>
#include <pprint.hpp>
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
auto getPPrint() {
  // std::stringstream ss;
  pprint::PrettyPrinter printer;
  printer.quotes(true);

  return printer;
}

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

auto get_Token() { std::error_code{errno, std::system_category()}; }

// 浮点型立即数
int main() {
  std::error_code error{};

  int a = 100;
  auto printer = getPPrint();
  printer.print(5);
}