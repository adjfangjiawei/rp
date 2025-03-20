#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

#include <cstring>

#include "Frontend/Parser/Lexer/LiteralsLexer/CharacterLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/NumberLiteralLexer.h"
#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"
#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"

namespace rp {
    namespace frontend {

        // 跳过空白字符和注释
        void Lexer::skipWhitespaceAndComments() {
            while (currentPos < sourceLength) {
                char c = source[currentPos];
                char next = (currentPos + 1 < sourceLength) ? source[currentPos + 1] : '\0';

                // 处理空白字符
                if (isspace(c)) {
                    if (c == '\n') {
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        currentColumn++;
                    }
                    currentPos++;
                    continue;
                }

                // 处理注释
                if (c == '/' && next == '/') {
                    // 单行注释
                    currentPos += 2;
                    currentColumn += 2;
                    while (currentPos < sourceLength && source[currentPos] != '\n') {
                        currentPos++;
                        currentColumn++;
                    }
                    continue;
                }

                if (c == '/' && next == '*') {
                    // 多行注释
                    currentPos += 2;
                    currentColumn += 2;
                    bool foundEnd = false;
                    while (currentPos < sourceLength - 1) {
                        if (source[currentPos] == '*' && source[currentPos + 1] == '/') {
                            currentPos += 2;
                            currentColumn += 2;
                            foundEnd = true;
                            break;
                        }
                        if (source[currentPos] == '\n') {
                            currentLine++;
                            currentColumn = 1;
                        } else {
                            currentColumn++;
                        }
                        currentPos++;
                    }
                    if (!foundEnd) {
                        // 未闭合的多行注释
                        reportError("Unterminated multi-line comment", currentLine, currentColumn);
                    }
                    continue;
                }

                // 不是空白字符或注释，退出循环
                break;
            }
        }

        std::string Lexer::getErrorContext(size_t line, size_t column, size_t context_lines) const {
            std::string result;
            size_t start_line = (line > context_lines) ? line - context_lines : 1;
            size_t end_line = line + context_lines;
            size_t current_line = 1;
            size_t pos = 0;

            while (pos < sourceLength && current_line <= end_line) {
                if (current_line >= start_line) {
                    // 添加行号
                    result += std::to_string(current_line) + " | ";

                    // 添加该行内容
                    while (pos < sourceLength && source[pos] != '\n') {
                        result += source[pos++];
                    }
                    result += '\n';

                    // 如果是错误所在行，添加错误指示符
                    if (current_line == line) {
                        result += "  | ";
                        for (size_t i = 1; i < column; ++i) {
                            result += ' ';
                        }
                        result += "^\n";
                    }
                }

                // 移动到下一行
                while (pos < sourceLength && source[pos] != '\n') {
                    pos++;
                }
                if (pos < sourceLength && source[pos] == '\n') {
                    pos++;
                }
                current_line++;
            }

            return result;
        }

        Token Lexer::getNextTokenFromSource() {
            // 跳过空白字符和注释
            skipWhitespaceAndComments();

            // 记录token的起始位置
            saveTokenStart();

            // 到达文件末尾
            if (currentPos >= sourceLength) {
                return createToken(TokenKind::EndOfFile);
            }

            try {
                char c = source[currentPos];
                char next = (currentPos + 1 < sourceLength) ? source[currentPos + 1] : '\0';
                char nextnext = (currentPos + 2 < sourceLength) ? source[currentPos + 2] : '\0';

                // 更新扫描器的位置
                scanner->setPosition(currentPos, currentLine, currentColumn);

                // 标识符或关键字
                if (scanner->isIdentifierStart(c)) {
                    Token token = scanner->scanIdentifier();
                    updatePositionFromScanner();
                    return token;
                }

                // 数字
                if (isdigit(c) || (c == '.' && currentPos + 1 < sourceLength && isdigit(source[currentPos + 1]))) {
                    NumberLiteralLexer numberLexer(this->diagnostics);
                    numberLexer.setSource(source, sourceLength, filename);
                    numberLexer.currentPos = currentPos;
                    numberLexer.currentLine = currentLine;
                    numberLexer.currentColumn = currentColumn;
                    Token token = numberLexer.scan();
                    currentPos = numberLexer.currentPos;
                    currentLine = numberLexer.currentLine;
                    currentColumn = numberLexer.currentColumn;
                    return token;
                }

                // 字符字面量
                if (c == '\'') {
                    CharacterLiteralLexer charLexer(this->diagnostics);
                    charLexer.setSource(source, sourceLength, filename);
                    charLexer.currentPos = currentPos;
                    charLexer.currentLine = currentLine;
                    charLexer.currentColumn = currentColumn;
                    Token token = charLexer.scan();
                    currentPos = charLexer.currentPos;
                    currentLine = charLexer.currentLine;
                    currentColumn = charLexer.currentColumn;
                    return token;
                }

                // 字符串字面量
                bool isStringStart = false;
                bool isRawString = false;
                size_t prefixLen = 0;

                // 检查各种字符串前缀
                if (c == '"') {
                    // 普通字符串字面量
                    isStringStart = true;
                } else if (currentPos + 1 < sourceLength) {
                    if (c == 'R' && next == '"') {
                        // R"..." 原始字符串字面量
                        isStringStart = true;
                        isRawString = true;
                        prefixLen = 1;
                    } else if ((c == 'L' || c == 'u' || c == 'U') && next == '"') {
                        // L"...", u"...", U"..." 宽字符串字面量
                        isStringStart = true;
                        prefixLen = 1;
                    } else if (currentPos + 2 < sourceLength) {
                        if ((c == 'L' || c == 'u' || c == 'U') && next == 'R' && nextnext == '"') {
                            // LR"...", uR"...", UR"..." 原始宽字符串字面量
                            isStringStart = true;
                            isRawString = true;
                            prefixLen = 2;
                        } else if (c == 'u' && next == '8' && nextnext == '"') {
                            // u8"..." UTF-8字符串字面量
                            isStringStart = true;
                            prefixLen = 2;
                        } else if (currentPos + 3 < sourceLength && c == 'u' && next == '8' && nextnext == 'R' &&
                                   source[currentPos + 3] == '"') {
                            // u8R"..." 原始UTF-8字符串字面量
                            isStringStart = true;
                            isRawString = true;
                            prefixLen = 3;
                        }
                    }
                }

                if (isStringStart) {
                    StringLiteralLexer stringLexer(diagnostics);
                    stringLexer.setSource(source, sourceLength, filename);
                    // 移动到实际的字符串内容开始位置
                    // 设置字符串词法分析器的初始位置
                    size_t stringStartPos = currentPos + prefixLen + (isRawString ? 0 : 0);
                    size_t stringStartColumn = currentColumn + prefixLen + (isRawString ? 0 : 0);
                    stringLexer.setSource(source, sourceLength, filename);
                    stringLexer.setPosition(stringStartPos, currentLine, stringStartColumn);
                    StringScanResult result = stringLexer.scan();

                    // 只有在成功解析时才更新位置
                    if (result.success) {
                        currentPos = stringLexer.getCurrentPos();
                        currentLine = stringLexer.getCurrentLine();
                        currentColumn = stringLexer.getCurrentColumn();
                        return result.token;
                    } else {
                        // 如果解析失败，使用错误恢复
                        Token errorToken(TokenKind::Invalid);
                        errorToken.setError(result.error,
                                            static_cast<unsigned int>(currentLine),
                                            static_cast<unsigned int>(currentColumn));

                        // 确保至少前进一个字符，防止死循环
                        if (currentPos == stringLexer.getCurrentPos()) {
                            currentPos++;
                            currentColumn++;
                        } else {
                            currentPos = stringLexer.getCurrentPos();
                            currentLine = stringLexer.getCurrentLine();
                            currentColumn = stringLexer.getCurrentColumn();
                        }

                        // 跳过剩余的字符串内容直到找到下一个引号或换行符
                        while (currentPos < sourceLength) {
                            if (source[currentPos] == '"' || source[currentPos] == '\n') {
                                currentPos++;
                                currentColumn++;
                                break;
                            }
                            currentPos++;
                            currentColumn++;
                        }

                        return errorToken;
                    }
                }

                // 运算符和标点符号
                Token token = scanner->scanOperatorOrPunctuation();
                updatePositionFromScanner();
                return token;

            } catch (const std::exception& e) {
                reportError(e.what(), currentLine, currentColumn);
                recoverFromError();
                return createToken(TokenKind::Invalid);
            }
        }

        void Lexer::fillTokenCache(size_t n) {
            while (tokenCache.size() < n) {
                Token token = getNextTokenFromSource();
                tokenCache.push_back(token);
                if (token.kind == TokenKind::EndOfFile) {
                    break;
                }
            }
        }

        void Lexer::reportError(const std::string& message, size_t line, size_t column) {
            std::string errorContext = getErrorContext(line, column);
            SourceLocation loc;
            loc.filename = filename;
            loc.line = line;
            loc.column = column;
            diagnostics->report(DiagnosticLevel::Error, loc, message + "\n" + errorContext);
        }

        void Lexer::recoverFromError() {
            // 增强的错误恢复：跳过直到找到下一个明确的token边界
            bool inString = false;
            bool inChar = false;
            bool inComment = false;
            bool inRawString = false;
            bool foundRawOpenParen = false;
            bool collectingRawDelimiter = false;
            std::string rawDelimiter;
            size_t startPos = currentPos;
            size_t startLine = currentLine;
            size_t startColumn = currentColumn;

            while (currentPos < sourceLength) {
                char c = source[currentPos];
                char next = (currentPos + 1 < sourceLength) ? source[currentPos + 1] : '\0';
                char prev = (currentPos > 0) ? source[currentPos - 1] : '\0';

                // 处理换行
                if (c == '\n') {
                    currentLine++;
                    currentColumn = 1;
                    // 换行只会终止单行注释和未终止的普通字符串/字符字面量
                    inComment = false;
                    if (!inRawString) {
                        inString = false;
                        inChar = false;
                    }
                } else {
                    currentColumn++;
                }

                // 处理原始字符串的特殊情况
                if (inRawString) {
                    if (!foundRawOpenParen) {
                        if (c == '(') {
                            foundRawOpenParen = true;
                            collectingRawDelimiter = false;
                        } else if (collectingRawDelimiter) {
                            if (isalnum(c) || c == '_') {
                                rawDelimiter += c;
                                if (rawDelimiter.length() > 16) {
                                    // 分隔符过长，认为是错误的，重置状态
                                    inRawString = false;
                                    rawDelimiter.clear();
                                    collectingRawDelimiter = false;
                                }
                            } else {
                                // 非法分隔符字符，重置状态
                                inRawString = false;
                                rawDelimiter.clear();
                                collectingRawDelimiter = false;
                            }
                        }
                    } else {
                        // 检查是否找到结束序列
                        if (c == ')') {
                            size_t endPos = currentPos + 1;
                            bool isEnd = true;
                            // 检查分隔符
                            for (size_t i = 0; i < rawDelimiter.length(); i++) {
                                if (endPos + i >= sourceLength || source[endPos + i] != rawDelimiter[i]) {
                                    isEnd = false;
                                    break;
                                }
                            }
                            // 检查结束引号
                            if (isEnd && endPos + rawDelimiter.length() < sourceLength &&
                                source[endPos + rawDelimiter.length()] == '"') {
                                currentPos = endPos + rawDelimiter.length() + 1;
                                break;
                            }
                        }
                    }
                }

                // 检查是否找到了新的有效token开始
                if (!inString && !inChar && !inComment && !inRawString) {
                    // 检查各种token的开始标记
                    if (isalpha(c) || c == '_' ||                  // 标识符
                        isdigit(c) ||                              // 数字
                        c == '"' ||                                // 字符串
                        c == '\'' ||                               // 字符
                        c == '#' ||                                // 预处理指令
                        strchr("+-*/%<>=!&|^~.,:;()[]{}\\", c)) {  // 运算符和标点
                        break;
                    }
                }

                // 检查字符串和字符字面量的边界
                if (!inComment) {
                    if (c == '"' && prev != '\\') {
                        if (inString) {
                            inString = false;
                            currentPos++;  // 包含结束引号
                            break;
                        } else if (!inChar && !inRawString) {
                            break;  // 找到新的字符串开始
                        }
                    } else if (c == '\'' && prev != '\\') {
                        if (inChar) {
                            inChar = false;
                            currentPos++;  // 包含结束引号
                            break;
                        } else if (!inString && !inRawString) {
                            break;  // 找到新的字符字面量开始
                        }
                    }
                }

                // 检查注释
                if (!inString && !inChar && !inRawString && c == '/' && next == '/') {
                    inComment = true;
                    currentPos++;  // 跳过第二个'/'
                }

                currentPos++;
            }

            // 确保至少前进了一个字符
            if (currentPos == startPos) {
                currentPos++;
                currentColumn++;
            }
        }

    }  // namespace frontend
}  // namespace rp
