#ifndef ESCAPESEQUENCEPROCESSOR_H
#define ESCAPESEQUENCEPROCESSOR_H

#include <stdint.h>

#include <optional>
#include <string>
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
            static std::optional<size_t> getExpectedLength(char escapeChar);

          private:
            // 处理各种转义序列
            static std::string processHexEscape(const std::string& source, size_t& currentPos, std::string& error);
            static std::string processOctalEscape(const std::string& source, size_t& currentPos, std::string& error);
            static std::string processUnicodeEscape(const std::string& source, size_t& currentPos, std::string& error);

            // 辅助方法
            static bool isValidUnicodeEscape(const std::string& source, size_t pos, size_t length);
            static bool isValidSurrogateCodePoint(uint32_t codepoint);
            static std::string getDetailedErrorMessage(const std::string& basicError);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // ESCAPESEQUENCEPROCESSOR_H
