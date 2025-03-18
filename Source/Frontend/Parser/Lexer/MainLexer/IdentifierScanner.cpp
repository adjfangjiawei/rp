
#include "IdentifierScanner.h"

#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"
#include "Frontend/Parser/Lexer/Unicode/Categories/UnicodeCategories.h"
#include "Frontend/Parser/Lexer/Unicode/Processing/UnicodeProcessing.h"
#include "Frontend/Parser/Lexer/Unicode/Unicode.h"

namespace rp {
    namespace frontend {

        Token IdentifierScanner::scanIdentifier() {
            if (!source || currentPos >= sourceLength) {
                return createToken(TokenKind::Invalid);
            }

            size_t tokenStart = currentPos;
            std::string identifier;
            identifier.reserve(32);  // 预分配一个合理的初始大小
            bool hasUTF8 = false;

            // 检查第一个字符
            if (!isIdentifierStart(source[currentPos])) {
                reportInvalidIdentifier("Invalid identifier start character");
                return createToken(TokenKind::Invalid);
            }

            try {
                // 处理第一个字符
                unsigned char firstChar = static_cast<unsigned char>(source[currentPos]);
                if (firstChar >= 128) {
                    // UTF-8字符
                    auto [codepoint, bytesConsumed] = getNextCodepoint();
                    if (codepoint == 0 || bytesConsumed == 0) {
                        reportInvalidUTF8("Invalid UTF-8 sequence at identifier start");
                        return createToken(TokenKind::Invalid);
                    }

                    if (!isUnicodeIdentifierStart(codepoint)) {
                        reportInvalidIdentifier("Invalid Unicode character at identifier start");
                        return createToken(TokenKind::Invalid);
                    }

                    std::string utf8Char = scanUTF8Sequence();
                    if (utf8Char.empty()) {
                        reportInvalidUTF8();
                        return createToken(TokenKind::Invalid);
                    }
                    identifier = utf8Char;
                    hasUTF8 = true;
                } else {
                    identifier += source[currentPos];
                    currentPos++;
                    currentColumn++;
                }

                // 扫描标识符的剩余部分
                while (currentPos < sourceLength) {
                    if (!checkIdentifierLength(identifier)) {
                        reportIdentifierTooLong();
                        return createToken(TokenKind::Invalid, identifier, tokenStart);
                    }

                    unsigned char c = static_cast<unsigned char>(source[currentPos]);
                    if (c < 128) {
                        // ASCII字符
                        if (!isIdentifierContinue(source[currentPos])) {
                            break;
                        }
                        identifier += source[currentPos];
                        currentPos++;
                        currentColumn++;
                    } else {
                        // UTF-8字符
                        auto [codepoint, bytesConsumed] = getNextCodepoint();
                        if (codepoint == 0 || bytesConsumed == 0) {
                            reportInvalidUTF8("Invalid UTF-8 sequence in identifier");
                            skipInvalidUTF8();
                            continue;
                        }

                        if (!isUnicodeIdentifierContinue(codepoint)) {
                            break;
                        }

                        std::string utf8Char = scanUTF8Sequence();
                        if (utf8Char.empty()) {
                            reportInvalidUTF8();
                            skipInvalidUTF8();
                            continue;
                        }
                        identifier += utf8Char;
                        hasUTF8 = true;
                    }
                }
            } catch (const std::exception&) {
                reportInvalidIdentifier("Unexpected error while scanning identifier");
                return createToken(TokenKind::Invalid, identifier, tokenStart);
            }

            // 验证标识符
            if (!validateIdentifier(identifier)) {
                reportInvalidIdentifier("Invalid identifier format");
                return createToken(TokenKind::Invalid, identifier, tokenStart);
            }

            // 检查标识符长度
            if (identifier.length() > MAX_IDENTIFIER_LENGTH) {
                reportIdentifierTooLong();
                return createToken(TokenKind::Invalid, identifier, tokenStart);
            }

            // 检查是否是关键字（只有纯ASCII标识符才可能是关键字）
            if (!hasUTF8) {
                TokenKind kind;
                if (KeywordManager::isKeyword(identifier, kind)) {
                    return createToken(kind, identifier, tokenStart);
                }
            }

            // 创建标识符token
            return createToken(TokenKind::Identifier, identifier, tokenStart);
        }

        bool IdentifierScanner::isIdentifierStart(char c) const {
            // 增强的输入验证
            if (!source || !*source || currentPos >= sourceLength) {
                return false;
            }

            try {
                // 处理ASCII字符
                unsigned char uc = static_cast<unsigned char>(c);
                if (uc < 128) {
                    return isalpha(c) || c == '_';
                }

                // 增强的UTF-8验证
                // 1. 确保有足够的字符可供UTF-8解码
                if (currentPos >= sourceLength) {
                    return false;
                }

                // 2. 预检查第一个字节的有效性
                if (!unicode::UnicodeCore::isValidUtf8FirstByte(uc)) {
                    return false;
                }

                // 3. 尝试解码UTF-8字符
                auto codepoint = tryPeekCodepoint();
                if (!codepoint.has_value()) {
                    return false;
                }

                // 4. 验证Unicode码点的有效性
                if (!unicode::UnicodeCore::isValidCodepoint(*codepoint)) {
                    return false;
                }

                return isUnicodeIdentifierStart(*codepoint);
            } catch (const std::exception&) {
                // 捕获所有可能的异常并安全处理
                return false;
            }
        }

        bool IdentifierScanner::isIdentifierContinue(char c) const {
            if (!source || currentPos >= sourceLength) {
                return false;
            }

            try {
                if (static_cast<unsigned char>(c) < 128) {
                    return isalnum(c) || c == '_';
                }

                auto codepoint = tryPeekCodepoint();
                return codepoint.has_value() && isUnicodeIdentifierContinue(*codepoint);
            } catch (const std::exception&) {
                return false;
            }
        }

        bool IdentifierScanner::isUnicodeIdentifierStart(uint32_t codepoint) const {
            try {
                // 检查是否是有效的标识符起始字符
                return unicode::UnicodeCategories::isIdentifierStart(codepoint) ||
                       // 允许一些额外的Unicode字符作为标识符起始
                       unicode::UnicodeCategories::isInCategory(
                           codepoint, unicode::UnicodeCategories::Category::Letter_Uppercase) ||
                       unicode::UnicodeCategories::isInCategory(
                           codepoint, unicode::UnicodeCategories::Category::Letter_Lowercase) ||
                       unicode::UnicodeCategories::isInCategory(codepoint,
                                                                unicode::UnicodeCategories::Category::Letter_Titlecase);
            } catch (const std::exception&) {
                return false;
            }
        }

        bool IdentifierScanner::isUnicodeIdentifierContinue(uint32_t codepoint) const {
            try {
                // 检查是否是有效的标识符继续字符
                return unicode::UnicodeCategories::isIdentifierContinue(codepoint) ||
                       // 允许一些额外的Unicode字符作为标识符继续
                       unicode::UnicodeCategories::isInCategory(codepoint,
                                                                unicode::UnicodeCategories::Category::Number_Decimal) ||
                       unicode::UnicodeCategories::isInCategory(
                           codepoint, unicode::UnicodeCategories::Category::Mark_NonSpacing) ||
                       unicode::UnicodeCategories::isInCategory(
                           codepoint, unicode::UnicodeCategories::Category::Mark_SpacingCombining);
            } catch (const std::exception&) {
                return false;
            }
        }

        std::string IdentifierScanner::scanUTF8Identifier() {
            try {
                return scanUTF8Sequence();
            } catch (const std::exception&) {
                return "";
            }
        }

        bool IdentifierScanner::validateIdentifier(const std::string& identifier) const {
            if (identifier.empty()) {
                return false;
            }

            // 检查长度限制
            if (!checkIdentifierLength(identifier)) {
                return false;
            }

            try {
                // 验证UTF-8编码和字符有效性
                size_t pos = 0;
                bool isFirst = true;
                while (pos < identifier.length()) {
                    size_t bytesConsumed;
                    std::string_view sv(identifier.data() + pos, identifier.length() - pos);
                    uint32_t codepoint = unicode::UnicodeEncoding::utf8ToCodePoint(sv, bytesConsumed);

                    if (codepoint == 0 || bytesConsumed == 0) {
                        return false;
                    }

                    if (isFirst) {
                        if (!isUnicodeIdentifierStart(codepoint)) {
                            return false;
                        }
                        isFirst = false;
                    } else {
                        if (!isUnicodeIdentifierContinue(codepoint)) {
                            return false;
                        }
                    }

                    pos += bytesConsumed;
                }

                return true;
            } catch (const std::exception&) {
                return false;
            }
        }

        bool IdentifierScanner::isValidIdentifierChar(char c) const {
            try {
                return isIdentifierContinue(c);
            } catch (const std::exception&) {
                return false;
            }
        }

        bool IdentifierScanner::checkIdentifierLength(const std::string& current) const {
            return current.length() < MAX_IDENTIFIER_LENGTH;
        }

        void IdentifierScanner::reportInvalidIdentifier(const std::string& reason) {
            std::string message = "Invalid identifier";
            if (!reason.empty()) {
                message += ": " + reason;
            }
            message +=
                "\nIdentifiers must start with a letter or underscore, followed by letters, numbers, or underscores";
            message += "\nUnicode characters are allowed if they are valid identifier characters";

            reportError(message);
        }

        void IdentifierScanner::reportIdentifierTooLong() {
            std::string message =
                "Identifier exceeds maximum length of " + std::to_string(MAX_IDENTIFIER_LENGTH) + " characters\n";
            message += "Consider using a shorter name or breaking it into smaller parts";

            reportError(message);
        }

    }  // namespace frontend
}  // namespace rp
