#pragma once
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "TokenKind.h"

namespace rp {
    namespace frontend {
        struct NumberValue;
        enum class NumberKind;

        // Token基础结构体
        struct Token {
            TokenKind kind;
            std::string textStorage;  // 存储实际的字符串内容
            std::string_view text;    // 指向源码中的原始文本或textStorage
            unsigned line;
            unsigned column;
            std::string filename;
            size_t endPos{0};  // Token结束位置

            // 字面量值联合体
            union {
                long long intValue;
                double floatValue;
                bool boolValue;
            };

            // 错误信息结构
            struct ErrorInfo {
                bool hasError{false};
                std::string message;
                unsigned errorLine{0};
                unsigned errorColumn{0};
            };
            std::shared_ptr<ErrorInfo> errorInfo;

            // 字符串信息结构
            struct StringInfo {
                bool isRawString{false};
                std::string delimiter;
                bool isWide{false};
                bool isUTF8{false};
                bool isUTF16{false};
                bool isUTF32{false};
            };
            std::shared_ptr<StringInfo> stringInfo;

            // 构造函数
            Token();
            explicit Token(TokenKind k);
            Token(TokenKind k, unsigned ln, unsigned col);
            Token(TokenKind k, size_t pos, unsigned ln, unsigned col);
            Token(TokenKind k, unsigned ln, unsigned col, const std::string& fname);

            // 拷贝和移动操作
            Token(const Token& other);
            Token& operator=(const Token& other);
            Token(Token&& other) noexcept;
            Token& operator=(Token&& other) noexcept;

            // 文本操作
            void setText(std::string_view sv);
            void setText(std::string&& str);
            void setText(const std::string& str);
            std::string_view getText() const;

            // 错误处理
            void setError(const std::string& message, unsigned errorLine = 0, unsigned errorColumn = 0);
            bool hasError() const;
            std::string getErrorMessage() const;
            std::optional<std::pair<unsigned, unsigned>> getErrorLocation() const;

            // 字符串信息操作
            void setStringInfo(bool isRaw = false, const std::string& delim = "");
            void setStringEncoding(bool isWide = false,
                                   bool isUTF8 = false,
                                   bool isUTF16 = false,
                                   bool isUTF32 = false);

            // getter函数
            bool isRawString() const;
            bool isWideString() const;
            bool isUTF8String() const;
            bool isUTF16String() const;
            bool isUTF32String() const;
            std::string getDelimiter() const;
            TokenKind getKind() const;
            unsigned getLine() const;
            unsigned getColumn() const;
            const std::string& getFilename() const;
            size_t getEndPos() const;
            void setEndPos(size_t pos);

            // 其他函数
            std::string getLocation() const;
            bool isKeyword() const;
            std::optional<long long> getIntValue() const;
            std::optional<double> getFloatValue() const;
            std::optional<bool> getBoolValue() const;

            // 比较操作符
            bool operator==(const Token& other) const;
            bool operator!=(const Token& other) const;
        };

    }  // namespace frontend
}  // namespace rp
