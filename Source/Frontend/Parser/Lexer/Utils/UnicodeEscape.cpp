#include "UnicodeEscape.h"
#include "Unicode.h"
#include <cctype>

namespace rp {
namespace frontend {

UnicodeEscape::Result UnicodeEscape::parseUnicodeEscape(std::string_view text) {
  Result result{false, "", 0, ""};

  if (text.length() < 2) {
    result.error = "Incomplete Unicode escape sequence";
    return result;
  }

  // 检查是否是Unicode转义序列
  if (text[0] != '\\' || (text[1] != 'u' && text[1] != 'U')) {
    result.error = "Invalid Unicode escape sequence";
    return result;
  }

  // 确定需要的十六进制位数
  size_t hexDigits = (text[1] == 'u') ? 4 : 8;
  if (text.length() < hexDigits + 2) {
    result.error = "Incomplete Unicode escape sequence";
    return result;
  }

  // 解析十六进制数字
  uint32_t codePoint = 0;
  for (size_t i = 0; i < hexDigits; ++i) {
    char c = text[i + 2];
    if (!isHexDigit(c)) {
      result.error = "Invalid hexadecimal digit in Unicode escape sequence";
      return result;
    }
    codePoint = (codePoint << 4) | hexDigitToInt(c);
  }

  // 验证码点的有效性
  if (codePoint > 0x10FFFF) {
    result.error = "Unicode code point out of range";
    return result;
  }

  result.success = true;
  result.value = Unicode::codePointToUtf8(codePoint);
  result.consumed = hexDigits + 2;
  return result;
}

UnicodeEscape::Result UnicodeEscape::parseCommonEscape(std::string_view text) {
  Result result{false, "", 0, ""};

  if (text.length() < 2 || text[0] != '\\') {
    result.error = "Invalid escape sequence";
    return result;
  }

  char c = text[1];
  result.consumed = 2;
  result.success = true;

  switch (c) {
  case 'n':
    result.value = "\n";
    break;
  case 't':
    result.value = "\t";
    break;
  case 'r':
    result.value = "\r";
    break;
  case '\"':
    result.value = "\"";
    break;
  case '\'':
    result.value = "'";
    break;
  case '\\':
    result.value = "\\";
    break;
  case '0':
    result.value = std::string(1, '\0');
    break;
  case 'a':
    result.value = "\a";
    break;
  case 'b':
    result.value = "\b";
    break;
  case 'f':
    result.value = "\f";
    break;
  case 'v':
    result.value = "\v";
    break;
  default:
    result.success = false;
    result.error = "Unknown escape sequence";
    result.consumed = 0;
  }

  return result;
}

UnicodeEscape::Result UnicodeEscape::parseOctalEscape(std::string_view text) {
  Result result{false, "", 0, ""};

  if (text.length() < 2 || text[0] != '\\') {
    result.error = "Invalid octal escape sequence";
    return result;
  }

  // 最多读取3个八进制数字
  uint32_t value = 0;
  size_t i = 1;
  size_t count = 0;

  while (i < text.length() && count < 3 && isOctalDigit(text[i])) {
    value = (value << 3) | (text[i] - '0');
    ++i;
    ++count;
  }

  if (count == 0) {
    result.error = "Invalid octal escape sequence";
    return result;
  }

  if (value > 255) {
    result.error = "Octal escape sequence out of range";
    return result;
  }

  result.success = true;
  result.value = std::string(1, static_cast<char>(value));
  result.consumed = count + 1;
  return result;
}

UnicodeEscape::Result UnicodeEscape::parseHexEscape(std::string_view text) {
  Result result{false, "", 0, ""};

  if (text.length() < 3 || text[0] != '\\' || text[1] != 'x') {
    result.error = "Invalid hexadecimal escape sequence";
    return result;
  }

  uint32_t value = 0;
  size_t i = 2;
  size_t count = 0;

  while (i < text.length() && count < 2 && isHexDigit(text[i])) {
    value = (value << 4) | hexDigitToInt(text[i]);
    ++i;
    ++count;
  }

  if (count == 0) {
    result.error = "Invalid hexadecimal escape sequence";
    return result;
  }

  result.success = true;
  result.value = std::string(1, static_cast<char>(value));
  result.consumed = count + 2;
  return result;
}

bool UnicodeEscape::isHexDigit(char c) { return std::isxdigit(c) != 0; }

bool UnicodeEscape::isOctalDigit(char c) { return c >= '0' && c <= '7'; }

int UnicodeEscape::hexDigitToInt(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return 0;
}

} // namespace frontend
} // namespace rp
