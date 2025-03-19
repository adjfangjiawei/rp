#include "StringLiteralUtils.h"

#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"

namespace rp {
    namespace frontend {

        bool StringLiteralUtils::isOctalDigit(char c) { return c >= '0' && c <= '7'; }

        bool StringLiteralUtils::isHexDigit(char c) {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
        }

        int StringLiteralUtils::hexDigitToInt(char c) {
            if (c >= '0' && c <= '9') return c - '0';
            if (c >= 'a' && c <= 'f') return c - 'a' + 10;
            if (c >= 'A' && c <= 'F') return c - 'A' + 10;
            return -1;
        }

        std::string StringLiteralUtils::unicodeToUTF8(unsigned int codepoint) {
            return rp::frontend::unicode::UnicodeEncoding::encodeUtf8(codepoint);
        }

        bool StringLiteralUtils::isValidUTF8StartByte(unsigned char c) {
            return rp::frontend::unicode::UnicodeEncoding::getMultiByteCharLength(c) > 0;
        }

        size_t StringLiteralUtils::getUTF8ByteCount(unsigned char c) {
            return rp::frontend::unicode::UnicodeEncoding::getMultiByteCharLength(c);
        }

        bool StringLiteralUtils::isValidUTF8ContinuationByte(unsigned char c) {
            return (c & 0xC0) == 0x80;  // 这个简单的检查可以保留，因为它是一个基本的位操作
        }

        std::tuple<uint32_t, size_t> StringLiteralUtils::getUTF8Char(const std::string& str, size_t pos) {
            return rp::frontend::unicode::UnicodeEncoding::getMultiByteChar(str, pos);
        }

        std::tuple<bool, size_t> StringLiteralUtils::validateUTF8Sequence(const std::string& str, size_t pos) {
            if (pos >= str.length()) {
                return {false, 0};
            }

            unsigned char firstByte = static_cast<unsigned char>(str[pos]);

            // ASCII字符
            if (firstByte < 0x80) {
                return {true, 1};
            }

            // 获取UTF-8序列长度
            size_t expectedLength = rp::frontend::unicode::UnicodeEncoding::getMultiByteCharLength(firstByte);
            if (expectedLength == 0 || pos + expectedLength > str.length()) {
                return {false, 0};
            }

            // 验证后续字节
            for (size_t i = 1; i < expectedLength; i++) {
                unsigned char continuationByte = static_cast<unsigned char>(str[pos + i]);
                if ((continuationByte & 0xC0) != 0x80) {
                    return {false, 0};
                }
            }

            // 验证完整的UTF-8序列
            auto [codepoint, actualLength] = rp::frontend::unicode::UnicodeEncoding::getMultiByteChar(str, pos);
            if (actualLength != expectedLength) {
                return {false, 0};
            }

            // 验证码点的有效范围
            if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                return {false, 0};
            }

            // 验证最小编码长度
            size_t minimumLength = 1;
            if (codepoint >= 0x80) minimumLength = 2;
            if (codepoint >= 0x800) minimumLength = 3;
            if (codepoint >= 0x10000) minimumLength = 4;

            if (expectedLength < minimumLength) {
                return {false, 0};
            }

            return {true, actualLength};
        }

    }  // namespace frontend
}  // namespace rp
