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
enum tokenType {

};

// 获得一个token
auto get_Token(std::string &source, int index) {
  // 跳过无效字符
  while (source[index] != ' ' && source[index] != '\n' &&
         source[index] != '\t') {
    index++;
  }

  if (isalpha(source[index])) {

  } else if (isdigit(source[index])) {
  }
}

/******************************************************************************/
// 处理整数
auto getTokenInteger(std::string source, int index) {}

// 处理科学计数法的整数
auto getTokenIntegerScientificNotation(std::string source, int index) {}

// 处理带引号的整数，形如1'111'111
auto getTokenIntegerWithQuotationMark(std::string source, int index) {}

// 处理中文的整数,形如一万三千五百零九，要考虑编码
auto getTokenIntegerChinese(std::string source, int index) {}

/******************************************************************************/
// 处理浮点
auto getTokenFloat(std::string source, int index) {}

// 处理中文的浮点数,形如一万三千五百零九点五，要考虑编码
auto getTokenFloatChinese(std::string source, int index) {}
/******************************************************************************/

/******************************************************************************/
// 处理字符
auto getTokenChar(std::string source, int index) {}

// 处理字符串
auto getTokenString(std::string source, int index) {}
/******************************************************************************/

/******************************************************************************/
// 处理变量名称
auto getTokenVariable(std::string source, int index) {}

// 处理变量名称
auto getTokenVariableChinese(std::string source, int index) {}

// 对色情反动以及不合法的变量名称发出警告
auto WarningTokenVariable(std::string source, int index) {}

// 对色情反动以及不合法的中文变量名称发出中文警告
auto WarningTokenVariableChinese(std::string source, int index) {}
/******************************************************************************/

/******************************************************************************/
// 要在语言内部支持日期与时间的国际化
// 处理日期
auto getTokenDate(std::string source, int index) {}

// 处理中文日期
auto getTokenDateChinese(std::string source, int index) {}

// 处理时间
auto getTokenTime(std::string source, int index) {}

// 处理中文时间
auto getTokenTimeChinese(std::string source, int index) {}
/******************************************************************************/

/******************************************************************************/
// 处理操作符
auto getTokenOperator(std::string source, int index) {}

// 处理赋值
auto getTokenAssignment(std::string source, int index) {}

// 处理等号赋值
auto getTokenAssignmentEqual(std::string source, int index) {}

// 处理is赋值
auto getTokenAssignmentIs(std::string source, int index) {}

// 处理has来处理组合类型
auto getTokenHas(std::string source, int index) {}

// 处理有来处理组合类型
auto getTokenHasChinese(std::string source, int index) {}

// 处理自动推导
auto getTokenAuto(std::string source, int index) {}
/******************************************************************************/
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