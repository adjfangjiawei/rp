#include "StringLiteralLexer.h"

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
                Token errorToken(TokenKind::Invalid);
                errorToken.setError("行号或列号超出有效范围", 0, 0);
                return errorToken;
            }

            // 获取对应的Token类型
            TokenKind kind = StringLiteralUtils::getPrefixTokenKind(prefix);

            // 创建Token
            Token token(kind, static_cast<unsigned>(currentLine), static_cast<unsigned>(currentColumn), filename);
            token.setText(content);

            // 设置字符串编码信息
            bool isWide = false;
            bool isUTF8 = false;
            bool isUTF16 = false;
            bool isUTF32 = false;
            bool isRaw = false;

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
                case StringPrefix::R:
                    isRaw = true;
                    break;
                case StringPrefix::LR:
                    isWide = true;
                    isRaw = true;
                    break;
                case StringPrefix::u8R:
                    isUTF8 = true;
                    isRaw = true;
                    break;
                case StringPrefix::uR:
                    isUTF16 = true;
                    isRaw = true;
                    break;
                case StringPrefix::UR:
                    isUTF32 = true;
                    isRaw = true;
                    break;
                default:
                    break;
            }

            token.setStringEncoding(isWide, isUTF8, isUTF16, isUTF32);
            if (isRaw) {
                token.setStringInfo(true);
            }

            return token;
        }

        bool StringLiteralLexer::validateString(const std::string& content,
                                                std::string& error,
                                                std::vector<std::string>& warnings) {
            try {
                // 检查长度限制
                if (content.length() > validationOptions.maxLength) {
                    error = "字符串超过最大长度限制（" + std::to_string(validationOptions.maxLength) + "字节）";
                    return false;
                }

                // 检查行数限制
                size_t lineCount = 1;
                size_t pos = 0;
                const size_t maxIterations = content.length() * 2;  // 防止死循环的安全限制
                size_t iterations = 0;

                while (pos < content.length() && iterations < maxIterations) {
                    iterations++;

                    // 确保pos不会越界
                    if (pos >= content.length()) {
                        error = "字符串处理时发生越界错误";
                        return false;
                    }

                    // 处理UTF-8字符
                    if ((unsigned char)content[pos] >= 0x80) {
                        // 检查剩余字节是否足够
                        if (pos >= content.length() - 1) {
                            error = "UTF-8序列不完整，在位置 " + std::to_string(pos);
                            return false;
                        }

                        try {
                            auto [codepoint, length] = unicode::UnicodeEncoding::getMultiByteChar(content, pos);
                            if (length == 0 || length > 4) {  // UTF-8最多4字节
                                error = "无效的UTF-8序列在位置 " + std::to_string(pos);
                                pos++;  // 确保至少前进一个字节，避免死循环
                                return false;
                            }
                            pos += length;
                        } catch (const std::exception& e) {
                            error = "处理UTF-8字符时发生错误：" + std::string(e.what());
                            pos++;  // 确保至少前进一个字节，避免死循环
                            return false;
                        }
                    } else {
                        if (content[pos] == '\n') {
                            lineCount++;
                            if (!validationOptions.allowMultiline) {
                                error = "不允许多行字符串";
                                return false;
                            }
                            if (lineCount > validationOptions.maxLines) {
                                error =
                                    "字符串超过最大行数限制（" + std::to_string(validationOptions.maxLines) + "行）";
                                return false;
                            }
                        }
                        pos++;
                    }
                }

                // 检查是否因为达到最大迭代次数而退出循环
                if (iterations >= maxIterations) {
                    error = "字符串处理超过最大迭代次数限制，可能存在无效的UTF-8序列";
                    return false;
                }

                // 检查UTF-8有效性
                if (validationOptions.strictUTF8) {
                    std::string utf8Error;
                    if (!StringLiteralUtils::validateCompleteUTF8String(content, utf8Error)) {
                        error = "字符串包含无效的UTF-8序列: " + utf8Error;
                        return false;
                    }
                }

                // 检查控制字符
                if (!validationOptions.allowControlChars) {
                    pos = 0;
                    while (pos < content.length()) {
                        unsigned char c = (unsigned char)content[pos];
                        if (c < 0x80) {  // 只检查ASCII范围的控制字符
                            if (c < 0x20 && c != '\n' && c != '\r' && c != '\t') {
                                warnings.push_back("字符串包含控制字符: ASCII " + std::to_string(c));
                            }
                            pos++;
                        } else {
                            // 跳过UTF-8多字节字符
                            auto [_, length] = unicode::UnicodeEncoding::getMultiByteChar(content, pos);
                            pos += (length > 0) ? length : 1;
                        }
                    }
                }

                return true;
            } catch (const std::exception& e) {
                error = "字符串验证时发生错误: " + std::string(e.what());
                return false;
            }
        }

        void StringLiteralLexer::handleDiagnostics(const std::string& error,
                                                   const std::vector<std::string>& warnings,
                                                   const SourceLocation& loc) {
            if (!error.empty()) {
                diagnostics->report(DiagnosticLevel::Error, loc, error);
            }

            for (const auto& warning : warnings) {
                diagnostics->report(DiagnosticLevel::Warning, loc, warning);
            }
        }

        StringScanResult StringLiteralLexer::scan() {
            StringScanResult result;
            result.success = false;
            result.hasWarnings = false;
            result.errorPosition = currentPos;

            if (currentPos >= sourceLength) {
                result.error = "无效的字符串字面量扫描源";
                result.token = Token(TokenKind::Invalid);
                return result;
            }

            // 保存起始位置信息
            size_t startLine = currentLine;
            size_t startColumn = currentColumn;
            SourceLocation startLoc{
                filename, static_cast<unsigned int>(startLine), static_cast<unsigned int>(startColumn)};

            // 解析前缀
            StringPrefix prefix = parsePrefix();
            bool isRawString = StringLiteralUtils::isRawStringPrefix(prefix);

            // 根据字符串类型选择相应的处理器
            if (isRawString) {
                auto rawResult = RawStringProcessor::processRawStringLiteral(source, currentPos, startLoc);
                if (!rawResult.success) {
                    result.error = rawResult.error;
                    result.token = rawResult.token;
                    result.warnings = std::move(rawResult.warnings);
                    result.hasWarnings = rawResult.hasWarnings;
                    handleDiagnostics(result.error, result.warnings, startLoc);
                    return result;
                }
                result.token = createStringToken(std::string(rawResult.token.getText()), prefix);
                result.warnings = std::move(rawResult.warnings);
            } else {
                auto normalResult = NormalStringProcessor::processNormalStringLiteral(source, currentPos, startLoc);
                if (!normalResult.success) {
                    result.error = normalResult.error;
                    result.token = normalResult.token;
                    result.warnings = std::move(normalResult.warnings);
                    result.hasWarnings = normalResult.hasWarnings;
                    handleDiagnostics(result.error, result.warnings, startLoc);
                    return result;
                }
                result.token = createStringToken(std::string(normalResult.token.getText()), prefix);
                result.warnings = std::move(normalResult.warnings);
            }

            // 验证最终内容
            std::string validationError;
            if (!validateString(std::string(result.token.getText()), validationError, result.warnings)) {
                result.error = validationError;
                result.token = Token(TokenKind::Invalid);
                handleDiagnostics(validationError, result.warnings, startLoc);
                return result;
            }

            // 设置最终的结果
            result.success = true;
            result.hasWarnings = !result.warnings.empty();

            // 处理诊断信息
            handleDiagnostics(result.error, result.warnings, startLoc);

            return result;
        }

    }  // namespace frontend
}  // namespace rp
