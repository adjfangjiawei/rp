#include "Unicode.h"
#include <array>
#include <unordered_set>

namespace rp {
namespace frontend {

// Unicode字符类别的实现
bool Unicode::isIdentifierStart(uint32_t codepoint) {
  // 基本拉丁字母
  if ((codepoint >= 0x0041 && codepoint <= 0x005A) || // A-Z
      (codepoint >= 0x0061 && codepoint <= 0x007A) || // a-z
      codepoint == 0x005F) {                          // _
    return true;
  }

  // 扩展拉丁字母
  if (codepoint >= 0x00C0 && codepoint <= 0x00D6)
    return true;
  if (codepoint >= 0x00D8 && codepoint <= 0x00F6)
    return true;
  if (codepoint >= 0x00F8 && codepoint <= 0x00FF)
    return true;

  // 其他Unicode字母
  Category cat = getCategory(codepoint);
  return cat == Category::Letter_Uppercase ||
         cat == Category::Letter_Lowercase ||
         cat == Category::Letter_Titlecase ||
         cat == Category::Letter_Modifier || cat == Category::Letter_Other;
}

bool Unicode::isIdentifierStart(std::string_view sv) {
  if (sv.empty())
    return false;

  size_t bytesRead;
  uint32_t codePoint = utf8ToCodePoint(sv, bytesRead);
  return isIdentifierStart(codePoint);
}

bool Unicode::isIdentifierContinue(uint32_t codepoint) {
  if (isIdentifierStart(codepoint))
    return true;

  // 数字
  if (codepoint >= 0x0030 && codepoint <= 0x0039)
    return true;

  // 其他Unicode数字和连接符号
  Category cat = getCategory(codepoint);
  return cat == Category::Number_Decimal || cat == Category::Number_Letter ||
         cat == Category::Number_Other;
}

bool Unicode::isIdentifierContinue(std::string_view sv) {
  if (sv.empty())
    return false;

  size_t bytesRead;
  uint32_t codePoint = utf8ToCodePoint(sv, bytesRead);
  return isIdentifierContinue(codePoint);
}

Unicode::Category Unicode::getCategory(uint32_t codepoint) {
  // 基本拉丁字母
  if (codepoint >= 0x0041 && codepoint <= 0x005A) // A-Z
    return Category::Letter_Uppercase;
  if (codepoint >= 0x0061 && codepoint <= 0x007A) // a-z
    return Category::Letter_Lowercase;

  // 数字
  if (codepoint >= 0x0030 && codepoint <= 0x0039) // 0-9
    return Category::Number_Decimal;

  // ASCII标点符号
  if (codepoint <= 0x007F) {
    if (codepoint >= 0x0020 && codepoint <= 0x002F)
      return Category::Punctuation_Other;
    if (codepoint >= 0x003A && codepoint <= 0x0040)
      return Category::Punctuation_Other;
    if (codepoint >= 0x005B && codepoint <= 0x0060)
      return Category::Punctuation_Other;
    if (codepoint >= 0x007B && codepoint <= 0x007E)
      return Category::Punctuation_Other;
  }

  // 空白字符
  if (isWhitespace(codepoint))
    return Category::Separator_Space;

  // TODO: 添加更多Unicode类别的判断
  return Category::Other_NotAssigned;
}

Unicode::Category Unicode::getCategory(std::string_view sv) {
  if (sv.empty())
    return Category::Other_NotAssigned;

  size_t bytesRead;
  uint32_t codePoint = utf8ToCodePoint(sv, bytesRead);
  return getCategory(codePoint);
}

bool Unicode::isWhitespace(uint32_t codepoint) {
  // 常见的空白字符
  static const std::unordered_set<uint32_t> whitespaceSet = {
      0x0009, // 水平制表符
      0x000A, // 换行
      0x000B, // 垂直制表符
      0x000C, // 换页
      0x000D, // 回车
      0x0020, // 空格
      0x0085, // 下一行
      0x00A0, // 不换行空格
      0x2000, // En Quad
      0x2001, // Em Quad
      0x2002, // En Space
      0x2003, // Em Space
      0x2004, // Three-Per-Em Space
      0x2005, // Four-Per-Em Space
      0x2006, // Six-Per-Em Space
      0x2007, // Figure Space
      0x2008, // Punctuation Space
      0x2009, // Thin Space
      0x200A, // Hair Space
      0x2028, // Line Separator
      0x2029, // Paragraph Separator
      0x202F, // Narrow No-Break Space
      0x205F, // Medium Mathematical Space
      0x3000  // Ideographic Space
  };

  return whitespaceSet.find(codepoint) != whitespaceSet.end();
}

bool Unicode::isWhitespace(std::string_view sv) {
  if (sv.empty())
    return false;

  size_t bytesRead;
  uint32_t codePoint = utf8ToCodePoint(sv, bytesRead);
  return isWhitespace(codePoint);
}

uint32_t Unicode::utf8ToCodePoint(std::string_view sv, size_t &bytesRead) {
  bytesRead = 0;
  if (sv.empty())
    return 0;

  uint8_t firstByte = static_cast<uint8_t>(sv[0]);
  uint32_t codePoint;

  if ((firstByte & UTF8_1BYTE_MASK) == 0) {
    // 1字节序列
    bytesRead = 1;
    return firstByte;
  } else if ((firstByte & UTF8_3BYTE_MASK) == UTF8_2BYTE_MASK) {
    // 2字节序列
    if (sv.length() < 2)
      return 0;
    bytesRead = 2;
    codePoint =
        ((firstByte & 0x1F) << 6) | (static_cast<uint8_t>(sv[1]) & 0x3F);
  } else if ((firstByte & UTF8_4BYTE_MASK) == UTF8_3BYTE_MASK) {
    // 3字节序列
    if (sv.length() < 3)
      return 0;
    bytesRead = 3;
    codePoint = ((firstByte & 0x0F) << 12) |
                ((static_cast<uint8_t>(sv[1]) & 0x3F) << 6) |
                (static_cast<uint8_t>(sv[2]) & 0x3F);
  } else if ((firstByte & UTF8_4BYTE_MASK) == UTF8_4BYTE_MASK) {
    // 4字节序列
    if (sv.length() < 4)
      return 0;
    bytesRead = 4;
    codePoint = ((firstByte & 0x07) << 18) |
                ((static_cast<uint8_t>(sv[1]) & 0x3F) << 12) |
                ((static_cast<uint8_t>(sv[2]) & 0x3F) << 6) |
                (static_cast<uint8_t>(sv[3]) & 0x3F);
  } else {
    return 0; // 无效的UTF-8序列
  }

  return codePoint;
}

std::string Unicode::codePointToUtf8(uint32_t codePoint) {
  std::string result;

  if (codePoint <= 0x7F) {
    // 1字节序列
    result.push_back(static_cast<char>(codePoint));
  } else if (codePoint <= 0x7FF) {
    // 2字节序列
    result.push_back(static_cast<char>(0xC0 | (codePoint >> 6)));
    result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
  } else if (codePoint <= 0xFFFF) {
    // 3字节序列
    result.push_back(static_cast<char>(0xE0 | (codePoint >> 12)));
    result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
    result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
  } else if (codePoint <= 0x10FFFF) {
    // 4字节序列
    result.push_back(static_cast<char>(0xF0 | (codePoint >> 18)));
    result.push_back(static_cast<char>(0x80 | ((codePoint >> 12) & 0x3F)));
    result.push_back(static_cast<char>(0x80 | ((codePoint >> 6) & 0x3F)));
    result.push_back(static_cast<char>(0x80 | (codePoint & 0x3F)));
  }

  return result;
}

} // namespace frontend
} // namespace rp
