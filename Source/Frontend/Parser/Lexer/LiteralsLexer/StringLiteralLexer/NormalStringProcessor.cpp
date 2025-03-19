#include "NormalStringProcessor.h"

#include "../StringLiteralLexer/EscapeSequenceProcessor.h"
#include "../StringLiteralLexer/StringLiteralUtils.h"
#include "Frontend/Parser/Lexer/Unicode/Encoding/UnicodeEncoding.h"

namespace rp {
    namespace frontend {

        Token NormalStringProcessor::processNormalStringLiteral(const std::string& source,
                                                                size_t& currentPos,
                                                                const SourceLocation& startLoc) {
            if (currentPos >= source.length()) {
                Token token(TokenKind::Invalid);
                token.setError("字符串必须以引号开始",
                               static_cast<unsigned int>(startLoc.line),
                               static_cast<unsigned int>(startLoc.column));
                return token;
            }

            // 检查开始引号类型
            std::string quoteType;
            if (source[currentPos] == '"' || source[currentPos] == '\'') {
                quoteType = std::string(1, source[currentPos]);
                currentPos++;
            } else {
                // 检查UTF-8编码的中文引号
                auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);
                if (length > 0) {
                    std::string quote = source.substr(currentPos, length);
                    if (quote == "「" || quote == "『" || quote == "'" || quote == "\"") {
                        quoteType = quote;
                        currentPos += length;
                    } else {
                        Token token(TokenKind::Invalid);
                        token.setError("字符串必须以引号开始（支持 \", ', 「, 『, \',\"）",
                                       static_cast<unsigned int>(startLoc.line),
                                       static_cast<unsigned int>(startLoc.column));
                        return token;
                    }
                } else {
                    Token token(TokenKind::Invalid);
                    token.setError("字符串必须以引号开始（支持 \", \', 「, 『, \', \"）",
                                   static_cast<unsigned int>(startLoc.line),
                                   static_cast<unsigned int>(startLoc.column));
                    return token;
                }
            }
            std::string content;
            bool inMultiLine = false;
            size_t lineCount = 0;
            const size_t MAX_LINES = 1000;  // 限制最大行数

            bool hasEscapeError = false;  // 添加标志来跟踪是否遇到过转义序列错误

            while (currentPos < source.length()) {
                // 处理字符串终止
                bool isEndQuote = false;
                if (quoteType == "\"" || quoteType == "'") {
                    isEndQuote =
                        source[currentPos] == quoteType[0] && !StringLiteralUtils::isEscaped(source, currentPos);
                    if (isEndQuote) currentPos++;
                } else {
                    // 检查UTF-8编码的中文结束引号
                    auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);
                    if (length > 0) {
                        std::string quote = source.substr(currentPos, length);
                        std::string expectedEndQuote;
                        if (quoteType == "「")
                            expectedEndQuote = "」";
                        else if (quoteType == "『")
                            expectedEndQuote = "』";
                        else if (quoteType == "'")
                            expectedEndQuote = "'";
                        else if (quoteType == "\"")
                            expectedEndQuote = "\"";

                        if (quote == expectedEndQuote) {
                            isEndQuote = true;
                            currentPos += length;
                        }
                    }
                }

                if (isEndQuote) {
                    Token token(TokenKind::StringLiteral);  // 创建一个有效的字符串token
                    token.setText(content);
                    token.line = static_cast<unsigned int>(startLoc.line);
                    token.column = static_cast<unsigned int>(startLoc.column);

                    if (hasEscapeError) {
                        // 提供更详细的警告信息
                        std::string warningMsg = "字符串包含语法问题但已修复：\n";
                        warningMsg += "- 包含无效的转义序列，已保留原始字符\n";
                        warningMsg += "- 字符串已正确终止，内容已被保存\n";
                        warningMsg += "建议检查字符串内容确保符合预期";
                        token.setError(warningMsg,
                                       static_cast<unsigned int>(startLoc.line),
                                       static_cast<unsigned int>(startLoc.column));
                    }
                    return token;
                }

                // 处理转义序列
                if (source[currentPos] == '\\') {
                    if (currentPos + 1 >= source.length()) {
                        Token token(TokenKind::Invalid);
                        token.setError("不完整的转义序列",
                                       static_cast<unsigned int>(startLoc.line),
                                       static_cast<unsigned int>(startLoc.column));
                        return token;
                    }

                    char next = source[currentPos + 1];

                    // 处理行继续符
                    if (next == '\n' || next == '\r') {
                        inMultiLine = true;
                        if (next == '\r' && currentPos + 2 < source.length() && source[currentPos + 2] == '\n') {
                            currentPos += 3;  // 跳过 \r\n
                        } else {
                            currentPos += 2;  // 跳过 \n 或 \r
                        }
                        lineCount++;
                        if (lineCount > MAX_LINES) {
                            // 尝试在最大行数限制处截断字符串
                            Token token(TokenKind::StringLiteral);
                            token.setText(content);
                            token.line = static_cast<unsigned int>(startLoc.line);
                            token.column = static_cast<unsigned int>(startLoc.column);

                            std::string warningMsg = "字符串超过最大行数限制(" + std::to_string(MAX_LINES) + "行)：\n";
                            warningMsg += "- 字符串已在最大行数处截断\n";
                            warningMsg += "- 建议拆分为多个较小的字符串\n";
                            warningMsg += "- 或使用字符串连接操作符";

                            token.setError(warningMsg,
                                           static_cast<unsigned int>(startLoc.line),
                                           static_cast<unsigned int>(startLoc.column));

                            // 尝试找到下一个引号位置
                            size_t nextQuotePos = source.find('"', currentPos);
                            if (nextQuotePos != std::string::npos) {
                                currentPos = nextQuotePos + 1;
                            }

                            return token;
                        }

                        // 跳过下一行开头的空白字符
                        while (currentPos < source.length() && StringLiteralUtils::isWhitespace(source[currentPos])) {
                            currentPos++;
                        }
                        continue;
                    }

                    // 处理转义序列
                    // 检查下一个字符是否是有效的转义字符
                    if (currentPos + 1 >= source.length()) {
                        Token token(TokenKind::Invalid);
                        token.setError("不完整的转义序列",
                                       static_cast<unsigned int>(startLoc.line),
                                       static_cast<unsigned int>(startLoc.column));
                        return token;
                    }

                    char nextChar = source[currentPos + 1];
                    if (!EscapeSequenceProcessor::isValidEscapeSequence(nextChar)) {
                        // 记录详细的错误信息但继续扫描
                        hasEscapeError = true;  // 设置错误标志
                        std::string escapeError = "无效的转义序列 '\\" + std::string(1, nextChar) + "'";
                        if (StringLiteralUtils::isValidStringChar(nextChar)) {
                            escapeError += "，该字符不需要转义";
                        }
                        // 创建警告但继续处理
                        Token warningToken(TokenKind::StringLiteral);
                        warningToken.setError(
                            escapeError,
                            static_cast<unsigned int>(startLoc.line),
                            static_cast<unsigned int>(startLoc.column + (currentPos - startLoc.column)));

                        currentPos += 2;      // 跳过反斜杠和下一个字符
                        content += '\\';      // 保留原始的反斜杠
                        content += nextChar;  // 保留原始的字符
                        continue;
                    }

                    std::string escapeError;
                    size_t escapePos = currentPos;
                    std::string processed =
                        EscapeSequenceProcessor::processEscapeSequence(source, escapePos, escapeError);

                    if (!escapeError.empty()) {
                        // 记录错误但继续扫描，保留原始字符
                        hasEscapeError = true;  // 设置错误标志
                        currentPos += 2;        // 跳过反斜杠和下一个字符
                        content += '\\';        // 保留原始的反斜杠
                        content += nextChar;    // 保留原始的字符
                    } else {
                        content += processed;
                        currentPos = escapePos;
                    }
                    continue;
                }

                // 处理换行符
                if (source[currentPos] == '\n' || source[currentPos] == '\r') {
                    if (!inMultiLine) {
                        // 检查是否可能是忘记了行继续符
                        bool hasLeadingWhitespace = false;
                        size_t nextPos = currentPos + 1;
                        while (nextPos < source.length() && StringLiteralUtils::isWhitespace(source[nextPos])) {
                            nextPos++;
                            hasLeadingWhitespace = true;
                        }

                        // 如果下一行开头有引号，可能是想要一个多行字符串
                        if (nextPos < source.length() && source[nextPos] == '"') {
                            Token token(TokenKind::StringLiteral);
                            token.setText(content);
                            token.line = static_cast<unsigned int>(startLoc.line);
                            token.column = static_cast<unsigned int>(startLoc.column);

                            std::string warningMsg = "发现未转义的换行符：\n";
                            warningMsg += "- 如果需要多行字符串，请在行尾添加反斜杠\\\n";
                            warningMsg += "- 如果是不同的字符串，请确保正确终止当前字符串\n";
                            warningMsg += "已尝试将内容作为单个字符串处理";

                            token.setError(warningMsg,
                                           static_cast<unsigned int>(startLoc.line),
                                           static_cast<unsigned int>(startLoc.column));

                            // 更新位置到找到的引号之前
                            currentPos = nextPos;
                            return token;
                        } else {
                            // 尝试寻找下一个引号进行错误恢复
                            size_t nextQuotePos = source.find('"', currentPos);
                            if (nextQuotePos != std::string::npos) {
                                Token token(TokenKind::StringLiteral);
                                token.setText(content);
                                token.line = static_cast<unsigned int>(startLoc.line);
                                token.column = static_cast<unsigned int>(startLoc.column);

                                std::string warningMsg = "字符串中包含未转义的换行符：\n";
                                warningMsg += "- 已找到后续引号并尝试恢复\n";
                                warningMsg += "- 建议检查字符串格式是否符合预期";

                                token.setError(warningMsg,
                                               static_cast<unsigned int>(startLoc.line),
                                               static_cast<unsigned int>(startLoc.column));

                                currentPos = nextQuotePos + 1;
                                return token;
                            }

                            Token token(TokenKind::Invalid);
                            token.setError("字符串中包含未转义的换行符且找不到结束引号",
                                           static_cast<unsigned int>(startLoc.line),
                                           static_cast<unsigned int>(startLoc.column));
                            return token;
                        }
                    }

                    lineCount++;
                    if (lineCount > MAX_LINES) {
                        Token token(TokenKind::Invalid);
                        token.setError("字符串超过最大行数限制",
                                       static_cast<unsigned int>(startLoc.line),
                                       static_cast<unsigned int>(startLoc.column));
                        return token;
                    }

                    // 统一处理换行符
                    content += '\n';  // 将所有类型的换行符统一转换为 LF
                    if (source[currentPos] == '\r' && currentPos + 1 < source.length() &&
                        source[currentPos + 1] == '\n') {
                        currentPos += 2;  // 跳过 CRLF
                    } else {
                        currentPos++;  // 跳过 LF 或 CR
                    }

                    // 跳过下一行开头的空白字符
                    while (currentPos < source.length() && StringLiteralUtils::isWhitespace(source[currentPos])) {
                        currentPos++;
                    }
                    continue;
                }

                // 处理 UTF-8 字符
                unsigned char currentChar = static_cast<unsigned char>(source[currentPos]);
                if (currentChar >= 0x80) {
                    auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);
                    if (length == 0) {
                        // 尝试跳过无效的UTF-8序列并继续处理
                        hasEscapeError = true;

                        // 计算可能的UTF-8序列长度
                        size_t possibleLength = 1;
                        unsigned char firstByte = static_cast<unsigned char>(source[currentPos]);
                        if ((firstByte & 0xE0) == 0xC0)
                            possibleLength = 2;
                        else if ((firstByte & 0xF0) == 0xE0)
                            possibleLength = 3;
                        else if ((firstByte & 0xF8) == 0xF0)
                            possibleLength = 4;

                        std::string errorMsg = "发现无效的UTF-8序列：\n";
                        errorMsg += "- 位置: 第" + std::to_string(startLoc.line) + "行，第" +
                                    std::to_string(startLoc.column + (currentPos - startLoc.column)) + "列\n";
                        errorMsg += "- 字节值: ";

                        // 显示出错字节的十六进制值
                        for (size_t i = 0; i < possibleLength && (currentPos + i) < source.length(); ++i) {
                            char hex[4];
                            snprintf(hex, sizeof(hex), "%02X", static_cast<unsigned char>(source[currentPos + i]));
                            errorMsg += std::string(hex) + " ";
                        }

                        errorMsg += "\n- 该序列将被跳过，继续处理后续内容";

                        // 创建警告但继续处理
                        Token warningToken(TokenKind::StringLiteral);
                        warningToken.setError(
                            errorMsg,
                            static_cast<unsigned int>(startLoc.line),
                            static_cast<unsigned int>(startLoc.column + (currentPos - startLoc.column)));

                        // 跳过这个无效的UTF-8序列
                        currentPos += possibleLength;
                        content += "�";  // 添加替换字符
                        continue;
                    }

                    // 验证UTF-8序列是否在有效的Unicode范围内
                    if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                        hasEscapeError = true;
                        std::string errorMsg = "无效的Unicode码点：\n";
                        errorMsg += "- 码点值: U+" + std::to_string(codepoint) + "\n";
                        errorMsg += "- 该字符将被替换为替换字符";

                        Token warningToken(TokenKind::StringLiteral);
                        warningToken.setError(
                            errorMsg,
                            static_cast<unsigned int>(startLoc.line),
                            static_cast<unsigned int>(startLoc.column + (currentPos - startLoc.column)));

                        content += "�";  // 添加替换字符
                    } else {
                        content.append(source.substr(currentPos, length));
                    }
                    currentPos += length;
                    continue;
                }

                // 处理普通ASCII字符
                if (!StringLiteralUtils::isValidStringChar(currentChar)) {
                    // 记录详细的错误信息但继续扫描
                    hasEscapeError = true;  // 复用这个标志来表示字符串内容有错误
                    std::string charError = "发现无效字符: ";
                    if (currentChar < 0x20) {
                        charError += "控制字符 (ASCII " + std::to_string(static_cast<int>(currentChar)) + ")";
                    } else {
                        charError += "'" + std::string(1, currentChar) + "'";
                    }
                    charError += "，该字符将被忽略";

                    // 创建警告但继续处理
                    Token warningToken(TokenKind::StringLiteral);
                    warningToken.setError(charError,
                                          static_cast<unsigned int>(startLoc.line),
                                          static_cast<unsigned int>(startLoc.column + (currentPos - startLoc.column)));

                    currentPos++;  // 跳过无效字符
                    continue;
                }
                content += source[currentPos];
                currentPos++;
            }

            Token token(TokenKind::Invalid);
            token.setError("未终止的字符串字面量",
                           static_cast<unsigned int>(startLoc.line),
                           static_cast<unsigned int>(startLoc.column));
            return token;
        }

        bool NormalStringProcessor::validateStringContent(const std::string& str, std::string& error) {
            if (str.empty()) {
                return true;  // 允许空字符串
            }

            size_t i = 0;
            while (i < str.length()) {
                unsigned char c = static_cast<unsigned char>(str[i]);

                // 处理UTF-8字符
                if (c >= 0x80) {
                    auto [valid, bytesRead] = StringLiteralUtils::validateUTF8Sequence(str, i);
                    if (!valid) {
                        error = "无效的UTF-8序列";
                        return false;
                    }
                    i += bytesRead;
                    continue;
                }

                // 检查ASCII字符的有效性
                if (c < 0x20 && c != '\n' && c != '\r' && c != '\t') {
                    error = "控制字符必须通过转义序列表示";
                    return false;
                }

                if (c == 0x7F) {  // DEL字符
                    error = "无效的控制字符(DEL)";
                    return false;
                }

                if (!StringLiteralUtils::isValidStringChar(c)) {
                    error = "字符串中包含无效字符";
                    return false;
                }

                i++;
            }

            return true;
        }

    }  // namespace frontend
}  // namespace rp
