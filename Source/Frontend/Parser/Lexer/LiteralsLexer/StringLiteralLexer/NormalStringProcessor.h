
#pragma once

#include <string>
#include <string_view>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Token/TokenKind.h"

namespace rp {
    namespace frontend {

        // 字符串处理结果
        struct StringProcessResult {
            Token token;                        // 处理后的Token
            bool success;                       // 是否成功
            std::string error;                  // 错误信息
            size_t errorPosition;               // 错误位置
            bool hasWarnings;                   // 是否有警告
            std::vector<std::string> warnings;  // 警告信息列表
        };

        // 字符串引号类型
        enum class QuoteType {
            None,
            DoubleQuote,       // "
            SingleQuote,       // '
            ChineseQuote,      // 「」
            ChineseBookQuote,  // 『』
            SmartQuote,        // ''
            SmartDoubleQuote   // ""
        };

        class NormalStringProcessor {
          public:
            // 处理常规字符串字面量
            static StringProcessResult processNormalStringLiteral(const std::string& source,
                                                                  size_t& currentPos,
                                                                  const SourceLocation& startLoc);

            // 获取字符串的引号类型
            static QuoteType getQuoteType(const std::string& source, size_t pos, size_t& quoteLength);

          private:
            // 验证字符串内容的有效性
            static bool validateStringContent(const std::string& content,
                                              std::string& error,
                                              std::vector<std::string>& warnings);

            // 处理多行字符串
            static bool processMultiLineString(const std::string& source,
                                               size_t& currentPos,
                                               std::string& content,
                                               size_t& lineCount,
                                               std::vector<std::string>& warnings);

            // 处理UTF-8字符
            static bool processUTF8Char(const std::string& source,
                                        size_t& currentPos,
                                        std::string& content,
                                        std::vector<std::string>& warnings);

            // 获取匹配的结束引号
            static std::string getMatchingEndQuote(QuoteType quoteType);

            // 检查是否为有效的字符串字符
            static bool isValidChar(unsigned char c, bool allowNewline);

            // 常量定义
            static constexpr size_t MAX_LINES = 1000;            // 最大行数限制
            static constexpr size_t INITIAL_BUFFER_SIZE = 1024;  // 初始缓冲区大小
        };

    }  // namespace frontend
}  // namespace rp
