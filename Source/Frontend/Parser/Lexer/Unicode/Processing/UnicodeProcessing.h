
#pragma once
#include <cstdint>
#include <string>
#include <string_view>

namespace rp::frontend::unicode {

    class UnicodeProcessing {
      public:
        // 字符字面量处理
        struct CharacterResult {
            bool success;       // 是否成功
            uint32_t value;     // 字符值（码点）
            size_t consumed;    // 消耗的字节数
            std::string error;  // 错误信息
        };

        static CharacterResult processCharacter(const std::string &str, size_t start);
        static CharacterResult processUtf8Character(const std::string &str, size_t start);

        // 字符串字面量处理
        struct StringResult {
            bool success;       // 是否成功
            std::string value;  // 处理后的字符串
            size_t consumed;    // 消耗的字节数
            std::string error;  // 错误信息
        };

        static StringResult processUtf8String(const std::string &str, size_t start, size_t length);

        // Unicode正规化和转换
        static std::string normalize(const std::string &str, bool compose = true);
        static std::string toUpper(const std::string &str);
        static std::string toLower(const std::string &str);
        static std::string toTitleCase(const std::string &str);

        // UTF-8字符串操作
        static size_t utf8Length(const std::string &str);
        static std::string_view getNextUtf8Char(const std::string &str, size_t &pos);
        static bool isValidUtf8(const std::string &str);

        // UTF-8序列验证
        static bool validateUtf8Sequence(const std::string &str, size_t start, size_t &bytesConsumed);
        static bool validateUtf8String(const std::string &str, std::string &error);

        // Unicode字符宽度
        static int getCharWidth(uint32_t codepoint);
    };

}  // namespace rp::frontend::unicode
