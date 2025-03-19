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

        // 转义序列类型
        enum class EscapeType {
            Basic,    // 基本转义序列 (\n, \t, \r, \\, \", \')
            Hex,      // 十六进制转义序列 (\x)
            Octal,    // 八进制转义序列 (\0-\7)
            Unicode,  // Unicode转义序列 (\u)
            Extended  // 扩展Unicode转义序列 (\U)
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
        static bool isOctalDigit(char c);
        static uint32_t octalDigitToValue(char c);

      private:
        // 内部处理方法
        static EscapeResult parseBasicEscape(char c);
        static EscapeResult parseHexEscape(const std::string& input, size_t start, size_t length);
        static EscapeResult parseOctalEscape(const std::string& input, size_t start);
        static EscapeResult parseUnicodeEscape(const std::string& input, size_t start);
        static EscapeResult parseExtendedUnicodeEscape(const std::string& input, size_t start);
    };

}  // namespace rp::frontend::unicode
