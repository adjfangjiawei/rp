#ifndef RAWSTRINGPROCESSOR_H
#define RAWSTRINGPROCESSOR_H

#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "PrefixProcessor.h"
#include "StringProcessorCommon.h"
namespace rp {
    namespace frontend {

        // 位置信息结构体（与NormalStringProcessor保持一致）
        struct RawPositionInfo {
            size_t line;
            size_t column;

            RawPositionInfo(size_t l = 1, size_t c = 1) : line(l), column(c) {}
        };

        struct RawStringResult {
            Token token;                        // 处理后的Token
            bool success;                       // 是否成功
            std::string error;                  // 错误信息
            size_t errorPosition;               // 错误位置
            bool hasWarnings;                   // 是否有警告
            std::vector<std::string> warnings;  // 警告信息列表
            size_t consumed;                    // 消耗的字符数
            RawPositionInfo endPos;             // 结束位置信息
        };

        struct DelimiterResult {
            bool success;
            size_t consumed;
            std::string delimiter;
            std::string error;
            RawPositionInfo newPos;  // 处理后的位置信息
        };

        struct ClosingSequenceResult {
            bool success;
            size_t consumed;
            size_t contentConsumed;
            std::string content;
            std::string error;
            RawPositionInfo newPos;  // 处理后的位置信息
            size_t lineCount;        // 处理的行数
        };

        class RawStringProcessor {
          public:
            // 处理原始字符串字面量
            static RawStringResult processRawStringLiteral(const std::string& source,
                                                           size_t startPos,
                                                           StringPrefix prefix,
                                                           const SourceLocation& startLoc);

          private:
            // 验证分隔符
            static bool isValidDelimiter(const std::string& delimiter);

            // 解析分隔符
            static DelimiterResult parseDelimiter(const std::string& source,
                                                  size_t startPos,
                                                  RawPositionInfo& currentPos);

            // 查找结束序列
            static ClosingSequenceResult findClosingSequence(const std::string& source,
                                                             size_t startPos,
                                                             const std::string& delimiter,
                                                             RawPositionInfo& currentPos);

            // 处理UTF-8字符
            static UTF8ProcessResult processUTF8Char(const std::string& source, size_t startPos);

            // 验证字符串内容
            static bool validateContent(const std::string& content,
                                        std::string& error,
                                        std::vector<std::string>& warnings);

            // 更新位置信息
            static void updatePosition(RawPositionInfo& pos, char c);

            // 常量定义
            static constexpr size_t MAX_DELIMITER_LENGTH = 16;
            static constexpr size_t MAX_LINES = 1000;
            static constexpr size_t INITIAL_BUFFER_SIZE = 1024;
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RAWSTRINGPROCESSOR_H
