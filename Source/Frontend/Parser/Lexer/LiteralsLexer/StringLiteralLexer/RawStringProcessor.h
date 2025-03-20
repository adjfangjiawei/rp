
#ifndef RAWSTRINGPROCESSOR_H
#define RAWSTRINGPROCESSOR_H

#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"

namespace rp {
    namespace frontend {

        struct RawStringResult {
            Token token;                        // 处理后的Token
            bool success;                       // 是否成功
            std::string error;                  // 错误信息
            size_t errorPosition;               // 错误位置
            bool hasWarnings;                   // 是否有警告
            std::vector<std::string> warnings;  // 警告信息列表
        };

        class RawStringProcessor {
          public:
            // 处理原始字符串字面量
            static RawStringResult processRawStringLiteral(const std::string& source,
                                                           size_t& currentPos,
                                                           const SourceLocation& startLoc);

          private:
            // 验证分隔符
            static bool isValidDelimiter(const std::string& delimiter);

            // 查找结束序列
            static bool findClosingSequence(const std::string& source,
                                            size_t& currentPos,
                                            const std::string& delimiter,
                                            size_t& endPos);

            // 处理UTF-8字符
            static bool processUTF8Char(const std::string& source,
                                        size_t& currentPos,
                                        std::string& content,
                                        std::vector<std::string>& warnings);

            // 验证字符串内容
            static bool validateContent(const std::string& content,
                                        std::string& error,
                                        std::vector<std::string>& warnings);

            // 常量定义
            static constexpr size_t MAX_DELIMITER_LENGTH = 16;
            static constexpr size_t MAX_LINES = 1000;
            static constexpr size_t INITIAL_BUFFER_SIZE = 1024;
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RAWSTRINGPROCESSOR_H
