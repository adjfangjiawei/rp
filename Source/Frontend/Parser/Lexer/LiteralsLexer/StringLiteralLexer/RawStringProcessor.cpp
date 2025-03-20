#include "RawStringProcessor.h"

#include <sstream>

#include "Frontend/Parser/Lexer/Unicode/Core/UnicodeCore.h"
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"
#include "Frontend/Parser/Lexer/Unicode/Processing/UnicodeProcessing.h"
#include "Frontend/Parser/Lexer/Unicode/Unicode.h"
#include "StringLiteralUtils.h"

namespace rp {
    namespace frontend {

        RawStringResult RawStringProcessor::processRawStringLiteral(const std::string& source,
                                                                    size_t& currentPos,
                                                                    const SourceLocation& startLoc) {
            RawStringResult result;
            result.success = false;
            result.hasWarnings = false;
            result.errorPosition = currentPos;

            // 检查基本条件
            if (currentPos >= source.length()) {
                result.error = "原始字符串不完整";
                result.token = Token(TokenKind::Invalid);
                return result;
            }

            // 检查开始引号
            if (source[currentPos] != '"') {
                result.error = "原始字符串必须以 R\" 开始";
                result.token = Token(TokenKind::Invalid);
                return result;
            }
            currentPos++;

            // 解析分隔符
            std::string delimiter;
            bool foundOpenParen = false;
            size_t delimiterStart = currentPos;

            while (currentPos < source.length()) {
                char c = source[currentPos];
                if (c == '(') {
                    foundOpenParen = true;
                    currentPos++;
                    break;
                }

                if (!isValidDelimiter(std::string(1, c))) {
                    result.error = "无效的原始字符串分隔符字符: '" + std::string(1, c) + "'";
                    result.token = Token(TokenKind::Invalid);
                    return result;
                }

                delimiter += c;
                currentPos++;

                if (delimiter.length() > MAX_DELIMITER_LENGTH) {
                    result.error = "原始字符串分隔符过长（最大长度为" + std::to_string(MAX_DELIMITER_LENGTH) + "）";
                    result.token = Token(TokenKind::Invalid);
                    return result;
                }
            }

            if (!foundOpenParen) {
                result.error = "原始字符串缺少开始括号";
                result.token = Token(TokenKind::Invalid);
                return result;
            }

            // 收集字符串内容
            std::string content;
            content.reserve(INITIAL_BUFFER_SIZE);
            size_t lineCount = 0;
            size_t endPos;

            size_t contentStart = currentPos;
            bool foundClosing = false;

            while (currentPos < source.length()) {
                // 检查结束序列
                if (findClosingSequence(source, currentPos, delimiter, endPos)) {
                    foundClosing = true;
                    // 提取内容（不包括结束序列）
                    content = source.substr(contentStart, currentPos - contentStart);
                    currentPos = endPos;
                    break;
                }

                // 处理换行符
                if (source[currentPos] == '\n' || source[currentPos] == '\r') {
                    lineCount++;
                    if (lineCount > MAX_LINES) {
                        result.error = "原始字符串超过最大行数限制（" + std::to_string(MAX_LINES) + "行）";
                        result.token = Token(TokenKind::Invalid);
                        return result;
                    }
                }

                // 处理UTF-8字符
                if (static_cast<unsigned char>(source[currentPos]) >= 0x80) {
                    if (!processUTF8Char(source, currentPos, content, result.warnings)) {
                        result.hasWarnings = true;
                    }
                } else {
                    currentPos++;
                }
            }

            if (!foundClosing) {
                result.error = "原始字符串未找到匹配的结束序列 )" + delimiter + "\"";
                result.token = Token(TokenKind::Invalid);
                return result;
            }

            // 验证最终内容
            std::string validationError;
            if (!validateContent(content, validationError, result.warnings)) {
                if (!validationError.empty()) {
                    result.warnings.push_back(validationError);
                    result.hasWarnings = true;
                }
            }

            // 创建成功的Token
            result.token = Token(TokenKind::RawStringLiteral);
            result.token.setText(content);
            result.token.line = static_cast<unsigned int>(startLoc.line);
            result.token.column = static_cast<unsigned int>(startLoc.column);
            result.success = true;

            // 设置原始字符串信息
            result.token.setStringInfo(true, delimiter);

            return result;
        }

        bool RawStringProcessor::isValidDelimiter(const std::string& delimiter) {
            for (char c : delimiter) {
                if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
                    return false;
                }
            }
            return true;
        }

        bool RawStringProcessor::findClosingSequence(const std::string& source,
                                                     size_t& currentPos,
                                                     const std::string& delimiter,
                                                     size_t& endPos) {
            if (source[currentPos] != ')') {
                return false;
            }

            std::string closingSequence = ")" + delimiter + "\"";
            size_t remainingLength = source.length() - currentPos;

            if (remainingLength < closingSequence.length()) {
                return false;
            }

            // 尝试匹配完整的结束序列
            for (size_t i = 0; i < closingSequence.length(); ++i) {
                if (source[currentPos + i] != closingSequence[i]) {
                    return false;
                }
            }

            // 验证结束序列后没有紧跟着分隔符的有效字符
            size_t afterClosing = currentPos + closingSequence.length();
            if (afterClosing < source.length()) {
                char nextChar = source[afterClosing];
                if (isValidDelimiter(std::string(1, nextChar))) {
                    return false;
                }
            }

            endPos = currentPos + closingSequence.length();
            return true;
        }

        bool RawStringProcessor::processUTF8Char(const std::string& source,
                                                 size_t& currentPos,
                                                 std::string& content,
                                                 std::vector<std::string>& warnings) {
            auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);
            if (length == 0) {
                warnings.push_back("无效的UTF-8序列，使用替换字符");
                content += "�";
                currentPos++;
                return false;
            }

            if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                warnings.push_back("无效的Unicode码点: U+" + std::to_string(codepoint));
                content += "�";
                currentPos += length;
                return false;
            }

            // 对于原始字符串，我们保留原始的UTF-8序列
            currentPos += length;
            return true;
        }

        bool RawStringProcessor::validateContent(const std::string& content,
                                                 std::string& error,
                                                 std::vector<std::string>& warnings) {
            // 对于原始字符串，我们主要验证以下几点：
            // 1. 基本的UTF-8有效性
            // 2. 行数限制
            // 3. 总长度检查

            // 检查UTF-8有效性
            if (!unicode::UnicodeProcessing::isValidUtf8(content)) {
                warnings.push_back("字符串包含无效的UTF-8序列，但作为原始字符串将保持不变");
            }

            // 检查行数
            size_t lineCount = 1;
            for (char c : content) {
                if (c == '\n') {
                    lineCount++;
                    if (lineCount > MAX_LINES) {
                        error = "字符串超过最大行数限制";
                        return false;
                    }
                }
            }

            // 检查总长度
            if (content.length() > INITIAL_BUFFER_SIZE) {
                warnings.push_back("字符串长度超过建议的缓冲区大小，可能会影响性能");
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp