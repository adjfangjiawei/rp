#pragma once
#include <cstdint>
#include <string>

namespace rp::frontend::unicode {

    class UnicodeEscape {
      public:
        // 转义序列处理结果
        struct EscapeResult {
            bool success;        // 是否成功
            uint32_t codepoint;  // 解析出的码点
            size_t consumed;     // 消耗的字符数
            std::string error;   // 错误信息
        };

        // Unicode转义序列处理
        static EscapeResult parseEscapeSequence(const std::string& input, size_t start);

        // Unicode转义序列生成
        static std::string generateEscapeSequence(uint32_t codepoint);

        // 验证转义序列
        static bool validateEscapeSequence(const std::string& input, std::string& error);

        // 处理行继续符
        static EscapeResult processLineContinuation(const std::string& input);

        // 辅助函数
        static bool isHexDigit(char c);
        static uint32_t hexDigitToValue(char c);

      private:
        // 内部处理方法
        static EscapeResult parseHexEscape(const std::string& input, size_t start, size_t length);
        static EscapeResult parseUnicodeEscape(const std::string& input, size_t start);
        static EscapeResult parseExtendedUnicodeEscape(const std::string& input, size_t start);
    };

}  // namespace rp::frontend::unicode
