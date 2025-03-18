#pragma once
#include <cstdint>
#include <string>
#include <string_view>

namespace rp {
namespace frontend {

class UnicodeEscape {
public:
  // 解析转义序列的结果
  struct Result {
    bool success;      // 是否成功
    std::string value; // 解析后的字符串
    size_t consumed;   // 消耗的字符数
    std::string error; // 错误信息
  };

  // 解析Unicode转义序列
  // \u1234 或 \U12345678
  static Result parseUnicodeEscape(std::string_view text);

  // 解析通用转义序列
  // \n, \t, \r, \", \', \\, \0 等
  static Result parseCommonEscape(std::string_view text);

  // 解析八进制转义序列
  // \123
  static Result parseOctalEscape(std::string_view text);

  // 解析十六进制转义序列
  // \xFF
  static Result parseHexEscape(std::string_view text);

private:
  // 辅助函数
  static bool isHexDigit(char c);
  static bool isOctalDigit(char c);
  static int hexDigitToInt(char c);
};

} // namespace frontend
} // namespace rp
