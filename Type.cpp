#include <iostream>
#include <string>

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
class IntegerInstantValueExprAST : public InstantValueExprAST {
public:
  virtual void pprint(std::ostream &os) {}

private:
  int val;
};
int main() { std::cout << "hello world"; }