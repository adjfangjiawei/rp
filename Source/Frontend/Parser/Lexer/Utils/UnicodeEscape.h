#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>

namespace rp {
    namespace frontend {

        class UnicodeEscape {
          public:
            // 解析转义序列的结果
            struct Result {
                bool success;       // 是否成功
                std::string value;  // 解析后的字符串
                size_t consumed;    // 消耗的字符数
                std::string error;  // 错误信息
            };

            // 转义序列类型
            enum class EscapeType {
                Unicode,  // \u1234 或 \U12345678
                Common,   // \n, \t, \r 等
                Octal,    // \123
                Hex,      // \xFF
                Invalid   // 无效的转义序列
            };

            // 解析转义序列（统一入口）
            static Result parseEscapeSequence(std::string_view text);

            // 检测转义序列类型
            static EscapeType detectEscapeType(std::string_view text);

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

            // 验证转义序列
            static bool validateEscapeSequence(std::string_view text, std::string &error);

            // 转义字符串（将特殊字符转换为转义序列）
            static std::string escapeString(const std::string &str, bool escapeUnicode = false);

            // 解除转义（将转义序列转换为实际字符）
            static Result unescapeString(const std::string &str);

            // 处理行连续（处理字符串中的行连续转义）
            static Result processLineContinuation(std::string_view text);

          private:
            // 辅助函数
            static bool isHexDigit(char c);
            static bool isOctalDigit(char c);
            static int hexDigitToInt(char c);
            static int octalDigitToInt(char c);

            // 常用转义字符映射
            struct EscapeMapping {
                char escape_char;
                char actual_char;
            };
            static const EscapeMapping COMMON_ESCAPES[];

            // Unicode相关常量
            static constexpr size_t SHORT_UNICODE_LENGTH = 4;  // \u1234
            static constexpr size_t LONG_UNICODE_LENGTH = 8;   // \U12345678
            static constexpr uint32_t MAX_UNICODE_VALUE = 0x10FFFF;

            // 验证Unicode码点
            static bool isValidUnicodeCodepoint(uint32_t codepoint);

            // 解析固定长度的十六进制数
            static std::tuple<bool, uint32_t> parseHexNumber(std::string_view text, size_t length);
        };

    }  // namespace frontend
}  // namespace rp
