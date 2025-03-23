#ifndef STRINGLITERALUTILS_H
#define STRINGLITERALUTILS_H

#include <cstdint>
#include <string>
#include <tuple>

namespace rp {
    namespace frontend {

        enum class TokenKind;

        // 字符串字面量前缀类型
        enum class StringPrefix {
            None,  // 无前缀
            L,     // L"..."
            u,     // u"..."
            U,     // U"..."
            u8,    // u8"..."
            R,     // R"..."
            LR,    // LR"..."
            uR,    // uR"..."
            UR,    // UR"..."
            u8R    // u8R"..."
        };

        // 字符串错误类型
        enum class StringError {
            None,
            InvalidUTF8Sequence,
            UnterminatedString,
            InvalidEscapeSequence,
            InvalidDelimiter,
            InvalidPrefix
        };

        class StringLiteralUtils {
          public:
            // 现有的方法
            static bool isOctalDigit(char c);
            static bool isHexDigit(char c);
            static int hexDigitToInt(char c);
            static std::string unicodeToUTF8(unsigned int codepoint);

            // UTF-8 相关功能
            static bool isValidUTF8StartByte(unsigned char c);
            static size_t getUTF8ByteCount(unsigned char c);
            static bool isValidUTF8ContinuationByte(unsigned char c);
            static std::tuple<bool, size_t> validateUTF8Sequence(const std::string& str, size_t pos);
            static bool validateCompleteUTF8String(const std::string& str, std::string& errorMsg);

            // 获取UTF-8字符及其长度
            static std::tuple<uint32_t, size_t> getUTF8Char(const std::string& str, size_t pos);

            // 字符串前缀相关功能
            static std::tuple<StringPrefix, size_t> parseStringPrefix(const std::string& input);
            static bool isValidStringPrefix(const std::string& prefix);
            static bool isRawStringPrefix(StringPrefix prefix);
            static std::string getPrefixString(StringPrefix prefix);

            // 获取前缀对应的Token类型
            static TokenKind getPrefixTokenKind(StringPrefix prefix);

            // 字符串处理辅助方法
            static bool isWhitespace(char c);
            static bool isValidStringChar(unsigned char c);
            static bool hasValidQuotes(const std::string& str);
            static bool isEscaped(const std::string& str, size_t pos);
            static bool isUnescapedQuote(const std::string& str, size_t pos);

            // 错误处理方法
            static std::string getErrorMessage(StringError error, size_t pos = 0, const std::string& context = "");

            // 验证原始字符串分隔符
            static bool isValidRawStringDelimiter(const std::string& delimiter);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // STRINGLITERALUTILS_H
