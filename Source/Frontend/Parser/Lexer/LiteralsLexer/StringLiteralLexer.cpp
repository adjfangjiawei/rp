#include "Frontend/Parser/Lexer/LiteralsLexer/StringLiteralLexer.h"

#include <sstream>

#include "Frontend/Parser/Lexer/Unicode/Unicode.h"

namespace rp {
    namespace frontend {

        StringPrefix StringLiteralLexer::parsePrefix() {
            if (currentPos >= sourceLength) {
                return StringPrefix::None;
            }

            std::string input = source.substr(currentPos, std::min(size_t(4), sourceLength - currentPos));
            auto [prefix, length] = StringLiteralUtils::parseStringPrefix(input);

            // 更新位置
            currentPos += length;
            currentColumn += length;

            return prefix;
        }

        Token StringLiteralLexer::createStringToken(const std::string& content, StringPrefix prefix) {
            // 检查数值范围以确保安全转换
            if (currentLine > std::numeric_limits<unsigned int>::max() ||
                currentColumn > std::numeric_limits<unsigned int>::max()) {
                Token errorToken(TokenKind::Invalid, 0, 0, filename);
                errorToken.setError(std::string("Line or column number too large"));
                return errorToken;
            }

            // 检查数值范围以确保安全转换
            if (currentLine > std::numeric_limits<unsigned>::max() ||
                currentColumn > std::numeric_limits<unsigned>::max()) {
                Token errorToken(TokenKind::Invalid, 0, 0, filename);
                errorToken.setError("Line or column number too large");
                return errorToken;
            }

            Token token(TokenKind::StringLiteral,
                        static_cast<unsigned>(currentLine),
                        static_cast<unsigned>(currentColumn),
                        filename);
            token.setText(content);

            // 设置字符串信息
            bool isWide = false;
            bool isUTF8 = false;
            bool isUTF16 = false;
            bool isUTF32 = false;

            switch (prefix) {
                case StringPrefix::L:
                    isWide = true;
                    break;
                case StringPrefix::u8:
                    isUTF8 = true;
                    break;
                case StringPrefix::u:
                    isUTF16 = true;
                    break;
                case StringPrefix::U:
                    isUTF32 = true;
                    break;
                default:
                    break;
            }

            token.setStringEncoding(isWide, isUTF8, isUTF16, isUTF32);
            return token;
        }

        std::string StringLiteralLexer::processEscapes(const std::string& input, bool& success) {
            std::string result;
            size_t pos = 0;
            std::string error;

            while (pos < input.length()) {
                if (input[pos] == '\\') {
                    if (pos + 1 >= input.length()) {
                        reportError("Incomplete escape sequence at end of string");
                        success = false;
                        return "";
                    }

                    char nextChar = input[pos + 1];
                    std::string processed;

                    // 处理通用转义序列
                    if (nextChar == 'n' || nextChar == 't' || nextChar == 'r' || nextChar == '\\' || nextChar == '\'' ||
                        nextChar == '\"' || nextChar == 'a' || nextChar == 'b' || nextChar == 'f' || nextChar == 'v') {
                        switch (nextChar) {
                            case 'n':
                                processed = "\n";
                                break;
                            case 't':
                                processed = "\t";
                                break;
                            case 'r':
                                processed = "\r";
                                break;
                            case '\\':
                                processed = "\\";
                                break;
                            case '\'':
                                processed = "'";
                                break;
                            case '\"':
                                processed = "\"";
                                break;
                            case 'a':
                                processed = "\a";
                                break;
                            case 'b':
                                processed = "\b";
                                break;
                            case 'f':
                                processed = "\f";
                                break;
                            case 'v':
                                processed = "\v";
                                break;
                        }
                        pos += 2;
                    }
                    // 处理Unicode转义序列
                    else if (nextChar == 'u' || nextChar == 'U') {
                        size_t codeLength = (nextChar == 'u') ? 4 : 8;
                        if (pos + 2 + codeLength > input.length()) {
                            reportError("Incomplete Unicode escape sequence");
                            success = false;
                            return "";
                        }

                        // 解析Unicode码点
                        uint32_t codepoint = 0;
                        for (size_t i = 0; i < codeLength; i++) {
                            char hexChar = input[pos + 2 + i];
                            if (!StringLiteralUtils::isHexDigit(hexChar)) {
                                reportError("Invalid hexadecimal digit in Unicode escape sequence");
                                success = false;
                                return "";
                            }
                            codepoint = (codepoint << 4) | StringLiteralUtils::hexDigitToInt(hexChar);
                        }

                        // 验证码点并转换为UTF-8
                        if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
                            reportError("Invalid Unicode code point");
                            success = false;
                            return "";
                        }

                        processed = unicode::UnicodeEncoding::encodeUtf8(codepoint);
                        pos += 2 + codeLength;
                    }
                    // 处理十六进制转义序列
                    else if (nextChar == 'x') {
                        if (pos + 4 > input.length()) {
                            reportError("Incomplete hexadecimal escape sequence");
                            success = false;
                            return "";
                        }

                        // 解析两位十六进制数
                        char hex1 = input[pos + 2];
                        char hex2 = input[pos + 3];
                        if (!StringLiteralUtils::isHexDigit(hex1) || !StringLiteralUtils::isHexDigit(hex2)) {
                            reportError("Invalid hexadecimal digit in escape sequence");
                            success = false;
                            return "";
                        }

                        unsigned char value =
                            (StringLiteralUtils::hexDigitToInt(hex1) << 4) | StringLiteralUtils::hexDigitToInt(hex2);
                        processed = std::string(1, static_cast<char>(value));
                        pos += 4;
                    }
                    // 处理八进制转义序列
                    else if (StringLiteralUtils::isOctalDigit(nextChar)) {
                        unsigned char value = 0;
                        size_t i = 0;
                        while (i < 3 && pos + 1 + i < input.length() &&
                               StringLiteralUtils::isOctalDigit(input[pos + 1 + i])) {
                            value = (value << 3) | (input[pos + 1 + i] - '0');
                            i++;
                        }
                        processed = std::string(1, static_cast<char>(value));
                        pos += 1 + i;
                    } else {
                        reportError("Unknown escape sequence: \\" + std::string(1, nextChar));
                        success = false;
                        return "";
                    }

                    result += processed;
                } else {
                    // 处理UTF-8字符
                    if ((unsigned char)input[pos] >= 0x80) {
                        auto [codepoint, bytes] = unicode::UnicodeEncoding::getMultiByteChar(input, pos);
                        if (bytes == 0) {
                            reportError("Invalid UTF-8 sequence in string literal");
                            success = false;
                            return "";
                        }
                        result += input.substr(pos, bytes);
                        pos += bytes;
                    } else {
                        result += input[pos];
                        pos++;
                    }
                }
            }

            success = true;
            return result;
        }

        void StringLiteralLexer::reportError(const std::string& message) {
            diagnostics->report(
                DiagnosticLevel::Error,
                SourceLocation{
                    filename, static_cast<unsigned int>(currentLine), static_cast<unsigned int>(currentColumn)},
                message);
        }

        Token StringLiteralLexer::scan() {
            if (currentPos >= sourceLength) {
                // 检查数值范围以确保安全转换
                if (currentLine > std::numeric_limits<unsigned int>::max() ||
                    currentColumn > std::numeric_limits<unsigned int>::max()) {
                    Token errorToken(TokenKind::Invalid, 0, 0, filename);
                    errorToken.setError(std::string("Line or column number too large"));
                    return errorToken;
                }

                // 检查数值范围以确保安全转换
                if (currentLine > std::numeric_limits<unsigned>::max() ||
                    currentColumn > std::numeric_limits<unsigned>::max()) {
                    Token errorToken(TokenKind::Invalid, 0, 0, filename);
                    errorToken.setError("Line or column number too large");
                    return errorToken;
                }

                Token token(TokenKind::Invalid,
                            static_cast<unsigned>(currentLine),
                            static_cast<unsigned>(currentColumn),
                            filename);
                token.setError("Invalid source for string literal scanning");
                return token;
            }

            // 保存起始位置信息
            size_t startLine = currentLine;
            size_t startColumn = currentColumn;

            // 解析前缀
            StringPrefix prefix = parsePrefix();
            bool isRawString = StringLiteralUtils::isRawStringPrefix(prefix);

            // 确保下一个字符是引号
            if (currentPos >= sourceLength || source[currentPos] != '"') {
                Token token(TokenKind::Invalid,
                            static_cast<unsigned int>(startLine),
                            static_cast<unsigned int>(startColumn),
                            filename);
                token.setError("Expected opening quote for string literal");
                return token;
            }
            currentPos++;  // 跳过开始引号
            currentColumn++;

            std::stringstream contentStream;
            bool inString = true;

            while (currentPos < sourceLength && inString) {
                char c = source[currentPos];

                if (isRawString) {
                    // 原始字符串处理
                    if (c == '"') {
                        currentPos++;
                        currentColumn++;
                        inString = false;
                    } else if (c == '\n') {
                        contentStream << c;
                        currentPos++;
                        currentLine++;
                        currentColumn = 1;
                    } else {
                        contentStream << c;
                        currentPos++;
                        currentColumn++;
                    }
                } else {
                    // 普通字符串处理
                    if (c == '"' && (currentPos == 0 || source[currentPos - 1] != '\\')) {
                        currentPos++;
                        currentColumn++;
                        inString = false;
                    } else if (c == '\n') {
                        if (currentPos > 0 && source[currentPos - 1] == '\\') {
                            // 处理字符串续行
                            if (currentPos + 1 >= sourceLength) {
                                Token token(TokenKind::Invalid, startLine, startColumn, filename);
                                token.setError("Unterminated string literal at end of file");
                                return token;
                            }

                            // 移除反斜杠
                            std::string current = contentStream.str();
                            if (!current.empty()) {
                                contentStream.str(current.substr(0, current.length() - 1));
                            }

                            currentPos++;  // 跳过换行符
                            currentLine++;
                            currentColumn = 1;

                            // 跳过行首空白字符
                            while (currentPos < sourceLength) {
                                char nextChar = source[currentPos];
                                if (nextChar == ' ' || nextChar == '\t') {
                                    currentPos++;
                                    currentColumn++;
                                } else {
                                    break;
                                }
                            }
                        } else {
                            Token token(TokenKind::Invalid, startLine, startColumn, filename);
                            token.setError("Unterminated string literal");
                            return token;
                        }
                    } else {
                        // 检查是否是有效的UTF-8字符或普通ASCII字符
                        if ((unsigned char)c >= 0x80) {
                            // 处理UTF-8多字节字符
                            auto [codepoint, bytes] = unicode::UnicodeEncoding::getMultiByteChar(source, currentPos);
                            if (bytes == 0) {
                                Token token(TokenKind::Invalid, startLine, startColumn, filename);
                                token.setError("Invalid UTF-8 sequence in string literal");
                                return token;
                            }
                            // 添加完整的UTF-8字符
                            std::string utf8char = unicode::UnicodeEncoding::encodeUtf8(codepoint);
                            contentStream << utf8char;
                            currentPos += bytes;
                            currentColumn += bytes;
                        } else if (c == '\\') {
                            // 特殊处理反斜杠，确保不会破坏UTF-8序列
                            if (currentPos + 1 < sourceLength) {
                                char nextChar = source[currentPos + 1];
                                if (nextChar == '\n') {
                                    // 处理字符串续行
                                    currentPos += 2;  // 跳过反斜杠和换行符
                                    currentLine++;
                                    currentColumn = 1;
                                    // 跳过下一行开头的空白字符
                                    while (currentPos < sourceLength &&
                                           (source[currentPos] == ' ' || source[currentPos] == '\t')) {
                                        currentPos++;
                                        currentColumn++;
                                    }
                                    continue;
                                }
                            }
                            contentStream << c;
                            currentPos++;
                            currentColumn++;
                        } else {
                            contentStream << c;
                            currentPos++;
                            currentColumn++;
                        }
                    }
                }
            }

            if (inString) {
                Token token(TokenKind::Invalid, startLine, startColumn, filename);
                token.setError("Unterminated string literal");
                return token;
            }

            std::string content = contentStream.str();

            // 处理转义序列（对于非原始字符串）
            if (!isRawString) {
                bool success;
                content = processEscapes(content, success);
                if (!success) {
                    Token token(TokenKind::Invalid, startLine, startColumn, filename);
                    token.setError("Failed to process escape sequences");
                    return token;
                }
            }

            // 验证UTF-8编码
            size_t pos = 0;
            while (pos < content.length()) {
                auto [valid, bytes] = StringLiteralUtils::validateUTF8Sequence(content, pos);
                if (!valid) {
                    Token token(TokenKind::Invalid, startLine, startColumn, filename);
                    token.setError("Invalid UTF-8 sequence in string literal at position " + std::to_string(pos) +
                                   " in string: " + content);
                    return token;
                }
                if (bytes == 0) {  // 额外的安全检查
                    Token token(TokenKind::Invalid, startLine, startColumn, filename);
                    token.setError("Zero-length UTF-8 sequence detected at position " + std::to_string(pos) +
                                   " in string: " + content);
                    return token;
                }
                pos += bytes;
            }

            // 创建并返回token
            // 安全地转换行号和列号
            if (startLine > std::numeric_limits<unsigned int>::max() ||
                startColumn > std::numeric_limits<unsigned int>::max()) {
                Token errorToken(TokenKind::Invalid, 0, 0, filename);
                errorToken.setError("Line or column number too large");
                return errorToken;
            }

            return createStringToken(content, prefix);
        }

    }  // namespace frontend
}  // namespace rp
