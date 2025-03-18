
#ifndef RP_UNICODE_PROCESSOR_H
#define RP_UNICODE_PROCESSOR_H

#include <cstdint>
#include <string>

namespace rp {
    namespace frontend {

        class UnicodeProcessor {
          public:
            // 处理Unicode转义序列
            static std::string processUnicodeEscape(const std::string& source,
                                                    size_t& currentPos,
                                                    bool isLongForm,
                                                    std::string& error);

            // 将Unicode码点转换为UTF-8字符串
            static std::string codePointToUTF8(uint32_t codepoint);

            // 验证Unicode码点的有效性
            static bool isValidCodePoint(uint32_t codepoint);

            // 处理十六进制转义序列
            static std::string processHexEscape(const std::string& source, size_t& currentPos, std::string& error);

          private:
            // 从字符串中解析十六进制数值
            static uint32_t parseHexValue(const std::string& hex, std::string& error);

            // 检查是否为有效的十六进制字符
            static bool isHexDigit(char c);

            // 获取十六进制字符的数值
            static int hexDigitToValue(char c);
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RP_UNICODE_PROCESSOR_H
