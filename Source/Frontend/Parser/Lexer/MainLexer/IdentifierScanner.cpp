
#include "IdentifierScanner.h"

#include "Frontend/Parser/Lexer/MainLexer/KeywordManager.h"
#include "Frontend/Parser/Lexer/Utils/Unicode.h"

namespace rp {
    namespace frontend {

        Token IdentifierScanner::scanIdentifier() {
            std::string identifier;
            bool hasUTF8 = false;
            size_t tokenStart = currentPos;  // 记录token的起始位置

            // 处理第一个字符
            unsigned char firstChar = static_cast<unsigned char>(source[currentPos]);
            if (firstChar >= 128) {
                // UTF-8字符
                identifier = scanUTF8Identifier();
                hasUTF8 = true;
            } else {
                identifier += source[currentPos];
                currentPos++;
                currentColumn++;
            }

            // 扫描标识符的剩余部分
            while (currentPos < sourceLength) {
                unsigned char c = static_cast<unsigned char>(source[currentPos]);

                if (c < 128) {
                    // ASCII字符
                    if (!isIdentifierPart(source[currentPos])) {
                        break;
                    }
                    identifier += source[currentPos];
                    currentPos++;
                    currentColumn++;
                } else {
                    // UTF-8字符
                    if (!isIdentifierPart(source[currentPos])) {
                        break;
                    }
                    std::string utf8Char = scanUTF8Identifier();
                    if (utf8Char.empty()) {
                        // 处理错误的UTF-8序列
                        reportInvalidUTF8();
                        skipInvalidUTF8();
                        continue;
                    }
                    identifier += utf8Char;
                    hasUTF8 = true;
                }
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
            if (static_cast<unsigned char>(c) < 128) {
                return isalpha(c) || c == '_';
            }
            return Unicode::isIdentifierStart(c);
        }

        bool IdentifierScanner::isIdentifierPart(char c) const {
            if (static_cast<unsigned char>(c) < 128) {
                return isalnum(c) || c == '_';
            }
            return Unicode::isIdentifierContinue(c);
        }

        std::string IdentifierScanner::scanUTF8Identifier() { return scanUTF8Sequence(); }

    }  // namespace frontend
}  // namespace rp
