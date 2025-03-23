#include "NormalStringProcessor.h"

#include "../StringLiteralLexer/EscapeSequenceProcessor.h"
#include "../StringLiteralLexer/StringLiteralUtils.h"
#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Token/TokenKind.h"
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"
#include "Frontend/Parser/Lexer/Unicode/Unicode.h"
#include "TokenBase.h"

namespace rp {
    namespace frontend {

        void NormalStringProcessor::updatePosition(PositionInfo& pos, char c) {
            if (c == '\n') {
                pos.line++;
                pos.column = 1;
            } else if (c == '\r') {
                // 对于\r\n的情况，\n会在下一次调用时处理
                pos.line++;
                pos.column = 1;
            } else {
                pos.column++;
            }
        }

        std::tuple<QuoteType, size_t> NormalStringProcessor::getQuoteType(const std::string& source, size_t pos) {
            if (pos >= source.length()) {
                return {QuoteType::None, 0};
            }

            // 检查单字节引号
            if (source[pos] == '"') {
                return {QuoteType::DoubleQuote, 1};
            }
            if (source[pos] == '\'') {
                return {QuoteType::SingleQuote, 1};
            }

            // 检查多字节引号（中文引号和智能引号）
            if (pos + 2 >= source.length()) {
                return {QuoteType::None, 0};
            }

            // 检查UTF-8编码的中文引号
            unsigned char firstByte = static_cast<unsigned char>(source[pos]);
            if (firstByte >= 0xE0) {  // UTF-8多字节序列的开始
                auto [codepoint, bytesRead] = unicode::UnicodeEncoding::getMultiByteChar(source, pos);
                if (bytesRead > 0) {
                    switch (codepoint) {
                        case 0x300C:  // 「
                            return {QuoteType::ChineseQuote, bytesRead};
                        case 0x300E:  // 『
                            return {QuoteType::ChineseBookQuote, bytesRead};
                        case 0x2018:  // '
                            return {QuoteType::SmartQuote, bytesRead};
                        case 0x201C:  // "
                            return {QuoteType::SmartDoubleQuote, bytesRead};
                    }
                }
            }

            return {QuoteType::None, 0};
        }

        UTF8ProcessResult NormalStringProcessor::processUTF8Char(const std::string& source, size_t startPos) {
            UTF8ProcessResult result;
            result.success = false;
            result.consumed = 0;
            result.content = "";

            // 使用Unicode模块处理UTF-8字符
            auto [codepoint, bytesRead] = unicode::UnicodeEncoding::getMultiByteChar(source, startPos);

            if (bytesRead == 0) {
                result.error = "无效的UTF-8序列";
                return result;
            }

            // 验证码点的有效性
            if (!unicode::UnicodeCore::isValidCodepoint(codepoint)) {
                result.error = "无效的Unicode码点";
                return result;
            }

            // 获取原始UTF-8序列
            result.content = source.substr(startPos, bytesRead);
            result.success = true;
            result.consumed = bytesRead;
            result.content = codepoint;

            return result;
        }

        MultiLineResult NormalStringProcessor::processMultiLineString(const std::string& source,
                                                                      size_t startPos,
                                                                      PositionInfo& currentPos) {
            MultiLineResult result;
            result.success = true;
            result.consumed = 0;
            result.lineCount = 0;
            result.content = "";
            result.newPos = currentPos;  // 初始化为当前位置

            size_t pos = startPos;

            // 跳过反斜杠
            if (pos < source.length() && source[pos] == '\\') {
                updatePosition(result.newPos, '\\');
                pos++;
            }

            // 处理换行符
            if (pos < source.length()) {
                if (source[pos] == '\r') {
                    updatePosition(result.newPos, '\r');
                    pos++;
                    if (pos < source.length() && source[pos] == '\n') {
                        updatePosition(result.newPos, '\n');
                        pos++;
                    }
                } else if (source[pos] == '\n') {
                    updatePosition(result.newPos, '\n');
                    pos++;
                }
            }

            result.lineCount = 1;

            // 跳过前导空白字符，但保留一个缩进级别
            size_t indentLevel = 0;
            while (pos < source.length() && StringLiteralUtils::isWhitespace(source[pos])) {
                if (source[pos] == ' ') {
                    indentLevel++;
                    updatePosition(result.newPos, ' ');
                } else if (source[pos] == '\t') {
                    indentLevel += 4;
                    updatePosition(result.newPos, '\t');
                }
                pos++;
            }

            result.consumed = pos - startPos;
            currentPos = result.newPos;  // 更新当前位置
            return result;
        }

        StringProcessResult NormalStringProcessor::processNormalStringLiteral(const std::string& source,
                                                                              size_t startPos,
                                                                              StringPrefix prefix,
                                                                              const SourceLocation& startLoc) {
            // 创建一个新的Token，使用StringLiteralUtils来获取正确的TokenKind
            TokenKind tokenKind = StringLiteralUtils::getPrefixTokenKind(prefix);
            auto tokenTemp = new Token(tokenKind);
            StringProcessResult result{.token = *tokenTemp};
            result.success = false;
            result.consumed = 0;

            // 初始化位置信息
            PositionInfo currentPos(startLoc.line, startLoc.column);

            if (startPos >= source.length()) {
                result.error = "字符串必须以引号开始";
                result.errorPosition = startPos;
                result.endPos = currentPos;
                return result;
            }

            // 获取引号类型
            auto [quoteType, quoteLength] = getQuoteType(source, startPos);
            if (quoteType == QuoteType::None) {
                result.error = "字符串必须以引号开始（支持 \", ', 「, 『, ',\"）";
                result.errorPosition = startPos;
                result.endPos = currentPos;
                return result;
            }

            // 更新引号的位置信息
            for (size_t i = 0; i < quoteLength; i++) {
                updatePosition(currentPos, source[startPos + i]);
            }

            size_t pos = startPos + quoteLength;
            std::string content;
            bool hasEscapeError = false;
            bool inMultiLine = false;
            size_t lineCount = 0;
            size_t lastValidPos = pos;

            while (pos < source.length()) {
                // 检查结束引号
                auto [endQuoteType, endQuoteLength] = getQuoteType(source, pos);
                if (endQuoteType == quoteType && !StringLiteralUtils::isEscaped(source, pos)) {
                    // 成功找到结束引号
                    result.token.setText(source.substr(startPos, pos + endQuoteLength - startPos));
                    result.token.line = static_cast<unsigned int>(startLoc.line);
                    result.token.column = static_cast<unsigned int>(startLoc.column);
                    if (result.error == "" && !result.hasWarnings()) {
                        result.success = true;
                    }
                    result.consumed = pos + endQuoteLength - startPos;

                    // 更新结束引号的位置信息
                    for (size_t i = 0; i < endQuoteLength; i++) {
                        updatePosition(currentPos, source[pos + i]);
                    }
                    result.endPos = currentPos;

                    if (hasEscapeError) {
                        result.warnings.push_back("字符串包含语法问题但已修复：");
                        result.warnings.push_back("- 包含无效的转义序列，已保留原始字符");
                        result.warnings.push_back("- 字符串已正确终止，内容已被保存");
                    }

                    return result;
                }

                // 处理转义序列
                if (source[pos] == '\\') {
                    updatePosition(currentPos, '\\');

                    if (pos + 1 >= source.length()) {
                        result.error = "不完整的转义序列";
                        result.errorPosition = pos;
                        result.consumed = pos - startPos;
                        result.endPos = currentPos;
                        return result;
                    }

                    char next = source[pos + 1];
                    if (next == '\n' || next == '\r') {
                        // 处理行继续符
                        auto multiLineResult = processMultiLineString(source, pos, currentPos);
                        if (!multiLineResult.success || multiLineResult.lineCount > MAX_LINES) {
                            result.error = "字符串超过最大行数限制";
                            result.errorPosition = pos;
                            result.consumed = pos - startPos;
                            result.endPos = currentPos;
                            return result;
                        }

                        pos += multiLineResult.consumed;
                        content += multiLineResult.content;
                        lineCount += multiLineResult.lineCount;
                        inMultiLine = true;
                        lastValidPos = pos;
                        currentPos = multiLineResult.newPos;
                        continue;
                    }

                    // 处理其他转义序列
                    std::string escapeError;
                    size_t escapePos = pos;
                    std::string processed =
                        EscapeSequenceProcessor::processEscapeSequence(source, escapePos, escapeError);

                    if (!escapeError.empty()) {
                        hasEscapeError = true;
                        content += '\\';
                        updatePosition(currentPos, '\\');
                        if (pos + 1 < source.length() && source[pos + 1] != '\n' && source[pos + 1] != '\r') {
                            content += source[pos + 1];
                            updatePosition(currentPos, source[pos + 1]);
                        }
                        pos += 2;
                        result.warnings.push_back("在位置 " + std::to_string(escapePos) + " 发现无效的转义序列: \\" +
                                                  source[escapePos + 1]);
                    } else {
                        content += processed;
                        // 更新位置信息，对于转义序列，我们只移动一列
                        currentPos.column++;
                        pos = escapePos;
                        lastValidPos = pos;
                    }
                    continue;
                }

                // 处理换行符
                if (source[pos] == '\n' || source[pos] == '\r') {
                    // 遇到未转义的换行符时，继续解析多行字符串
                    result.token.line = static_cast<unsigned int>(startLoc.line);
                    result.token.column = static_cast<unsigned int>(startLoc.column);
                    result.error = "字符串中包含未转义的换行符";
                    result.errorPosition = pos;
                    result.consumed = lastValidPos - startPos;
                    result.endPos = currentPos;

                    result.warnings.push_back("在位置 " + std::to_string(pos) + " 发现未转义的换行符");
                    result.warnings.push_back("字符串内容在位置 " + std::to_string(lastValidPos) + " 之前是有效的");
                    result.warnings.push_back("提示：使用反斜杠(\\)来转义换行符，或使用多行字符串语法");

                    // 仍然继续查找就把换行符加入到字符串中
                    content += source[pos];
                    updatePosition(currentPos, source[pos]);
                    pos++;
                    continue;
                }

                // 处理UTF-8字符
                if (static_cast<unsigned char>(source[pos]) >= 0x80) {
                    auto utf8Result = processUTF8Char(source, pos);
                    if (!utf8Result.success) {
                        hasEscapeError = true;
                        content += '?';
                        currentPos.column++;
                        pos++;
                        result.warnings.push_back("在位置 " + std::to_string(pos) + " 发现无效的UTF-8序列，已替换为?");
                    } else {
                        content += utf8Result.content;
                        // UTF-8字符只增加一列，不管它有多少字节
                        currentPos.column++;
                        pos += utf8Result.consumed;
                        lastValidPos = pos;
                    }
                    continue;
                }

                // 处理普通ASCII字符
                if (!StringLiteralUtils::isValidStringChar(static_cast<unsigned char>(source[pos]))) {
                    hasEscapeError = true;
                    result.warnings.push_back("在位置 " + std::to_string(pos) + " 发现无效字符");
                }
                content += source[pos];
                updatePosition(currentPos, source[pos]);
                lastValidPos = pos;
                pos++;
            }

            // 如果到达这里，说明没有找到结束引号
            result.error = "未终止的字符串字面量";
            result.errorPosition = startPos;
            result.consumed = lastValidPos - startPos;
            result.endPos = currentPos;

            result.warnings.push_back("字符串从位置 " + std::to_string(startPos) + " 开始");
            result.warnings.push_back("最后有效位置在 " + std::to_string(lastValidPos));
            result.warnings.push_back("提示：确保字符串以匹配的引号结束");

            return result;
        }

    }  // namespace frontend
}  // namespace rp
