#ifndef ESCAPESEQUENCEPROCESSOR_H
#define ESCAPESEQUENCEPROCESSOR_H

#include <optional>
#include <string>

#include "StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        struct EscapeSequenceResult {
            std::string value;     // 处理后的字符串值
            size_t consumed;       // 消耗的字符数
            bool success;          // 是否成功
            std::string error;     // 错误信息
            size_t errorPosition;  // 错误位置
        };

        class EscapeSequenceProcessor {
          public:
            // 处理转义序列
            static std::string processEscapeSequence(const std::string& source, size_t& currentPos, std::string& error);

            // 检查是否为有效的转义字符
            static bool isValidEscapeSequence(char c);

            // 获取转义序列的预期长度（不包括前导反斜杠）
            static std::optional<size_t> getExpectedLength(char escapeChar) {
                switch (escapeChar) {
                    case 'x':
                        return 2;  // \xHH
                    case 'u':
                        return 4;  // \uHHHH
                    case 'U':
                        return 8;  // \UHHHHHHHH
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                        return 3;  // 最多3位八进制数
                    default:
                        return 1;  // 简单转义字符
                }
            }

          private:
            // 处理各种转义序列
            static std::string processHexEscape(const std::string& source, size_t& currentPos, std::string& error);
            static std::string processOctalEscape(const std::string& source, size_t& currentPos, std::string& error);
            static std::string processUnicodeEscape(const std::string& source, size_t& currentPos, std::string& error);

            // 辅助方法
            static bool isValidUnicodeEscape(const std::string& source, size_t pos, size_t length);
            static bool isValidSurrogateCodePoint(uint32_t codepoint) {
                return codepoint < 0xD800 || codepoint > 0xDFFF;
            }
            static std::string getDetailedErrorMessage(const std::string& basicError);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // ESCAPESEQUENCEPROCESSOR_H
