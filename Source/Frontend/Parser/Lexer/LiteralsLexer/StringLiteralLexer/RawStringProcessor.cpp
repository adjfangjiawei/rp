#include "RawStringProcessor.h"

#include <iostream>
#include <sstream>

#include "Frontend/Diagnostic/Diagnostic.h"
#include "Frontend/Parser/Lexer/Token/Token.h"
#include "Frontend/Parser/Lexer/Token/TokenKind.h"
#include "Frontend/Parser/Lexer/Unicode/Core/UnicodeCore.h"
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"
#include "Frontend/Parser/Lexer/Unicode/Processing/UnicodeProcessing.h"
#include "Frontend/Parser/Lexer/Unicode/Unicode.h"
#include "StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        void RawStringProcessor::updatePosition(RawPositionInfo& pos, char c) {
            if (c == '\n' || c == '\r') {
                pos.line++;
                pos.column = 1;
            } else {
                pos.column++;
            }
        }

        RawStringResult RawStringProcessor::processRawStringLiteral(const std::string& source,
                                                                    size_t startPos,
                                                                    StringPrefix prefix,
                                                                    const SourceLocation& startLoc) {
            // 使用StringLiteralUtils获取正确的TokenKind
            TokenKind tokenKind = StringLiteralUtils::getPrefixTokenKind(prefix);
            auto token = new Token(tokenKind, startLoc.line, startLoc.column, startLoc.filename);
            RawStringResult result{.token = *token};
            result.success = false;
            result.hasWarnings = false;
            result.consumed = 0;

            // 初始化位置信息
            RawPositionInfo currentPos(startLoc.line, startLoc.column);

            // 检查基本条件
            if (startPos >= source.length()) {
                result.error = "原始字符串不完整";
                result.errorPosition = startPos;
                result.endPos = currentPos;
                return result;
            }

            // 检查开始引号
            if (source[startPos] != '"') {
                result.error = "原始字符串必须以 R\" 开始";
                result.errorPosition = startPos;
                result.endPos = currentPos;
                return result;
            }
            updatePosition(currentPos, '"');

            // 解析分隔符
            auto delimiterResult = parseDelimiter(source, startPos + 1, currentPos);
            if (!delimiterResult.success) {
                result.error = delimiterResult.error;
                result.errorPosition = startPos + delimiterResult.consumed;
                result.consumed = delimiterResult.consumed;
                result.endPos = delimiterResult.newPos;
                return result;
            }

            size_t contentStart = startPos + 1 + delimiterResult.consumed;
            currentPos = delimiterResult.newPos;

            // 查找结束序列
            auto closingResult = findClosingSequence(source, contentStart, delimiterResult.delimiter, currentPos);
            if (!closingResult.success) {
                result.error = closingResult.error;
                result.errorPosition = contentStart + closingResult.consumed;
                result.consumed = contentStart - startPos + closingResult.consumed;
                result.endPos = closingResult.newPos;
                return result;
            }

            // 验证内容
            std::string contentError;
            if (!validateContent(closingResult.content, contentError, result.warnings)) {
                result.hasWarnings = true;
                if (!contentError.empty()) {
                    result.warnings.push_back(contentError);
                }
            }

            // 创建成功的Token
            result.token = Token(tokenKind, startLoc.line, startLoc.column, startLoc.filename);
            result.token.setText(source.substr(contentStart, closingResult.contentConsumed));
            result.token.setStringInfo(true, delimiterResult.delimiter);
            result.success = true;
            result.consumed = contentStart + closingResult.consumed - startPos;
            result.endPos = closingResult.newPos;

            return result;
        }

        DelimiterResult RawStringProcessor::parseDelimiter(const std::string& source,
                                                           size_t startPos,
                                                           RawPositionInfo& currentPos) {
            DelimiterResult result;
            result.success = false;
            result.consumed = 0;
            result.newPos = currentPos;

            size_t pos = startPos;
            while (pos < source.length() && source[pos] != '(') {
                if (!isValidDelimiter(std::string(1, source[pos]))) {
                    result.error = "无效的原始字符串分隔符字符: '" + std::string(1, source[pos]) + "'";
                    return result;
                }
                result.delimiter += source[pos];
                updatePosition(result.newPos, source[pos]);
                pos++;

                if (result.delimiter.length() > MAX_DELIMITER_LENGTH) {
                    result.error = "原始字符串分隔符过长（最大长度为" + std::to_string(MAX_DELIMITER_LENGTH) + "）";
                    return result;
                }
            }

            if (pos >= source.length() || source[pos] != '(') {
                result.error = "原始字符串缺少开始括号";
                return result;
            }
            updatePosition(result.newPos, '(');

            result.success = true;
            result.consumed = pos - startPos + 1;  // 包括'('
            return result;
        }

        ClosingSequenceResult RawStringProcessor::findClosingSequence(const std::string& source,
                                                                      size_t startPos,
                                                                      const std::string& delimiter,
                                                                      RawPositionInfo& currentPos) {
            ClosingSequenceResult result;
            result.success = false;
            result.consumed = 0;
            result.newPos = currentPos;
            result.lineCount = 0;
            result.contentConsumed = 0;

            std::string closingSequence = ")" + delimiter + "\"";
            size_t pos = startPos;

            while (pos < source.length()) {
                if (source[pos] == ')') {
                    // 检查是否匹配结束序列
                    if (pos + closingSequence.length() <= source.length()) {
                        bool matches = true;
                        for (size_t i = 0; i < closingSequence.length(); ++i) {
                            if (source[pos + i] != closingSequence[i]) {
                                matches = false;
                                break;
                            }
                        }
                        if (matches) {
                            result.content = source.substr(startPos, pos - startPos);
                            result.success = true;
                            result.consumed = pos + closingSequence.length() - startPos;

                            // 更新结束序列的位置信息
                            for (size_t i = 0; i < closingSequence.length(); ++i) {
                                updatePosition(result.newPos, closingSequence[i]);
                            }
                            return result;
                        } else {
                            updatePosition(result.newPos, source[pos]);
                            pos++;
                            result.contentConsumed++;
                            continue;
                        }
                    }
                }

                // 处理换行符
                if (source[pos] == '\n') {
                    updatePosition(result.newPos, '\n');
                    result.lineCount++;
                    pos++;
                    result.contentConsumed++;
                    if (result.lineCount > MAX_LINES) {
                        result.error = "原始字符串超过最大行数限制（" + std::to_string(MAX_LINES) + "行）";
                        result.consumed = pos - startPos;
                        return result;
                    }
                } else if (source[pos] == '\r') {
                    if (pos + 1 < source.length() && source[pos + 1] == '\n') {
                        pos += 2;
                        result.contentConsumed += 2;
                        updatePosition(result.newPos, '\n');
                    } else {
                        pos++;
                        result.contentConsumed++;
                        updatePosition(result.newPos, '\r');
                    }
                    result.lineCount++;
                    if (result.lineCount > MAX_LINES) {
                        result.error = "原始字符串超过最大行数限制（" + std::to_string(MAX_LINES) + "行）";
                        result.consumed = pos - startPos;
                        return result;
                    }
                } else if (static_cast<unsigned char>(source[pos]) >= 0x80) {
                    // 处理UTF-8字符
                    auto utf8Result = processUTF8Char(source, pos);
                    result.newPos.column++;  // UTF-8字符只增加一列
                    pos += utf8Result.consumed;
                    result.contentConsumed += utf8Result.consumed;
                } else {
                    updatePosition(result.newPos, source[pos]);
                    pos++;
                    result.contentConsumed++;
                }
            }

            result.error = "原始字符串未找到匹配的结束序列 " + closingSequence;
            result.consumed = pos - startPos;
            return result;
        }

        bool RawStringProcessor::isValidDelimiter(const std::string& delimiter) {
            return StringLiteralUtils::isValidRawStringDelimiter(delimiter);
        }

        UTF8ProcessResult RawStringProcessor::processUTF8Char(const std::string& source, size_t startPos) {
            UTF8ProcessResult result;
            result.success = false;
            result.consumed = 0;

            // 使用Unicode模块的功能处理UTF-8字符
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

            result.success = true;
            result.consumed = bytesRead;
            result.content = codepoint;
            return result;
        }

        bool RawStringProcessor::validateContent(const std::string& content,
                                                 std::string& error,
                                                 std::vector<std::string>& warnings) {
            // 使用Unicode模块验证字符串内容
            auto validationResult = unicode::validateString(content);

            if (!validationResult.valid) {
                // 收集所有错误信息
                std::stringstream errorStream;
                for (const auto& err : validationResult.errors) {
                    if (!errorStream.str().empty()) {
                        errorStream << "; ";
                    }
                    errorStream << err.message;
                    warnings.push_back("位置 " + std::to_string(err.position) + ": " + err.message);
                }
                error = errorStream.str();
                return false;
            }

            // 检查字符串长度
            if (content.length() > INITIAL_BUFFER_SIZE) {
                warnings.push_back("字符串长度超过建议的最大值（" + std::to_string(INITIAL_BUFFER_SIZE) + "字节）");
            }

            // 检查是否包含控制字符（除了换行符和制表符）
            for (size_t i = 0; i < content.length(); ++i) {
                unsigned char c = static_cast<unsigned char>(content[i]);
                if (c < 32 && c != '\n' && c != '\t' && c != '\r') {
                    warnings.push_back("字符串包含控制字符（ASCII " + std::to_string(c) + "）");
                }
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
