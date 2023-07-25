#include <iostream>
#include <pprint.hpp>
#include <sstream>
#include <string>

auto getPPrint() {
  // std::stringstream ss;
  pprint::PrettyPrinter printer;
  printer.quotes(true);

  return printer;
}

// 抽象类
class ExprAST {
public:
  virtual void pprint(std::ostream &os) = 0;
};

// 变量语法树
class VariableExprAST : public ExprAST {
public:
  virtual void pprint(std::ostream &os) = 0;
};

class InstantValueExprAST : public ExprAST {
  virtual void pprint(std::ostream &os) = 0;
};

// 一元表达式
class UnaryExprAST : public ExprAST {
public:
private:
  char op;
  ExprAST *expr;
};

// 整型变量
class IntegerVariableExprAST : public VariableExprAST {
public:
  virtual void pprint(std::ostream &os) {}

private:
  int val;
  std::string name;
};

// 整型立即数
template <class Print>
class IntegerInstantValueExprAST : public InstantValueExprAST {
public:
  InstantValueExprAST(int value, Print *printer) : printer(printer) {}
  virtual void pprint(std::ostream &os) {}

private:
  int val;
  Print *printer;
};
int main() {
  int a = 100;
  auto printer = getPPrint();
  printer.print(5);
}