#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace rp {
namespace frontend {

class Unicode {
public:
  // Unicode字符类别
  enum class Category {
    Letter_Uppercase,
    Letter_Lowercase,
    Letter_Titlecase,
    Letter_Modifier,
    Letter_Other,
    Number_Decimal,
    Number_Letter,
    Number_Other,
    Punctuation_Connector,
    Punctuation_Dash,
    Punctuation_Open,
    Punctuation_Close,
    Punctuation_Quote,
    Punctuation_Other,
    Symbol_Math,
    Symbol_Currency,
    Symbol_Modifier,
    Symbol_Other,
    Separator_Space,
    Separator_Line,
    Separator_Paragraph,
    Mark_NonSpacing,
    Mark_SpacingCombining,
    Mark_Enclosing,
    Other_Control,
    Other_Format,
    Other_Surrogate,
    Other_PrivateUse,
    Other_NotAssigned
  };

  // UTF-8编码/解码
  static std::string encodeUtf8(uint32_t codepoint);
  static bool decodeUtf8(const std::string &utf8,
                         std::vector<uint32_t> &codepoints);

  // UTF-8工具函数
  static uint32_t utf8ToCodePoint(std::string_view sv, size_t &bytesRead);
  static std::string codePointToUtf8(uint32_t codePoint);

  // Unicode属性检查
  static bool isIdentifierStart(uint32_t codepoint);
  static bool isIdentifierStart(std::string_view sv);
  static bool isIdentifierContinue(uint32_t codepoint);
  static bool isIdentifierContinue(std::string_view sv);
  static bool isWhitespace(uint32_t codepoint);
  static bool isWhitespace(std::string_view sv);
  static bool isDigit(uint32_t codepoint);
  static bool isHexDigit(uint32_t codepoint);
  static bool isAlpha(uint32_t codepoint);
  static bool isAlnum(uint32_t codepoint);

  // Unicode字符分类
  static Category getCategory(uint32_t codepoint);
  static Category getCategory(std::string_view sv);
  static bool isInCategory(uint32_t codepoint, Category category);

  // UTF-8字符串操作
  static size_t utf8Length(const std::string &str);
  static std::string_view getNextUtf8Char(const std::string &str, size_t &pos);
  static bool isValidUtf8(const std::string &str);

  // Unicode正规化
  static std::string normalize(const std::string &str, bool compose = true);

  // Unicode大小写转换
  static std::string toUpper(const std::string &str);
  static std::string toLower(const std::string &str);
  static std::string toTitleCase(const std::string &str);

  // Unicode字符宽度
  static int getCharWidth(uint32_t codepoint);

public:
  // UTF-8编码长度检查
  static size_t getUtf8ByteCount(uint32_t codepoint);

private:
  // UTF-8编码常量
  static constexpr uint8_t UTF8_1BYTE_MASK = 0x80;
  static constexpr uint8_t UTF8_2BYTE_MASK = 0xE0;
  static constexpr uint8_t UTF8_3BYTE_MASK = 0xF0;
  static constexpr uint8_t UTF8_4BYTE_MASK = 0xF8;

  static size_t getUtf8SequenceLength(unsigned char firstByte);

  // Unicode数据表查找
  static bool binarySearchUnicodeData(uint32_t codepoint, Category &category);

  // 辅助函数
  static bool isUtf8ContinuationByte(unsigned char byte);
  static bool isValidCodepoint(uint32_t codepoint);
};

} // namespace frontend
} // namespace rp
