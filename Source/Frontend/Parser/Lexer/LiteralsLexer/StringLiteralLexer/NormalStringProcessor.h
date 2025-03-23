#pragma once

#include <string>
#include <string_view>
#include <tuple>
#include <vector>

#include "PrefixProcessor.h"
#include "StringLiteralUtils.h"
#include "StringProcessorCommon.h"

namespace rp {
    namespace frontend {

        class Token;
        struct SourceLocation;

        // 位置信息结构体
        struct PositionInfo {
            size_t line;
            size_t column;

            PositionInfo(size_t l = 1, size_t c = 1) : line(l), column(c) {}
        };

        // 字符串处理结果
        struct StringProcessResult {
            Token& token;                                     // 处理后的Token
            bool success;                                     // 是否成功
            std::string error;                                // 错误信息
            size_t errorPosition;                             // 错误位置
            bool hasWarnings() { return !warnings.empty(); }  // 是否有警告
            std::vector<std::string> warnings;                // 警告信息列表
            size_t consumed;                                  // 消耗的字符数
            PositionInfo endPos;                              // 结束位置信息
        };

        // 多行字符串处理结果
        struct MultiLineResult {
            bool success;
            size_t consumed;
            std::string content;
            size_t lineCount;
            std::vector<std::string> warnings;
            PositionInfo newPos;  // 处理后的新位置信息
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
                                                                  size_t startPos,
                                                                  StringPrefix prefix,
                                                                  const SourceLocation& startLoc);

            // 获取字符串的引号类型
            static std::tuple<QuoteType, size_t> getQuoteType(const std::string& source, size_t pos);

          private:
            // 验证字符串内容的有效性
            static bool validateStringContent(const std::string& content,
                                              std::string& error,
                                              std::vector<std::string>& warnings);

            // 处理多行字符串
            static MultiLineResult processMultiLineString(const std::string& source,
                                                          size_t startPos,
                                                          PositionInfo& currentPos);

            // 处理UTF-8字符
            static UTF8ProcessResult processUTF8Char(const std::string& source, size_t startPos);

            // 获取匹配的结束引号
            static std::string getMatchingEndQuote(QuoteType quoteType);

            // 检查是否为有效的字符串字符
            static bool isValidChar(unsigned char c, bool allowNewline);

            // 更新位置信息
            static void updatePosition(PositionInfo& pos, char c);

            // 常量定义
            static constexpr size_t MAX_LINES = 1000;            // 最大行数限制
            static constexpr size_t INITIAL_BUFFER_SIZE = 1024;  // 初始缓冲区大小
        };

    }  // namespace frontend
}  // namespace rp
