
#include "TokenCreator.h"

#include <limits>

namespace rp {
    namespace frontend {

        Token TokenCreator::createStringToken(
            const std::string& content, StringPrefix prefix, size_t line, size_t column, const std::string& filename) {
            // 检查数值范围以确保安全转换
            if (!isValidPosition(line, column)) {
                Token errorToken(TokenKind::Invalid);
                errorToken.setError("行号或列号超出有效范围", 0, 0);
                return errorToken;
            }

            // 获取对应的Token类型
            TokenKind kind = getPrefixTokenKind(prefix);

            // 创建Token
            Token token(kind, static_cast<unsigned>(line), static_cast<unsigned>(column), filename);
            token.setText(content);

            // 设置字符串编码信息
            setStringEncoding(token, prefix);

            return token;
        }

        void TokenCreator::setStringEncoding(Token& token, StringPrefix prefix) {
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
        }

        TokenKind TokenCreator::getPrefixTokenKind(StringPrefix prefix) {
            switch (prefix) {
                case StringPrefix::None:
                    return TokenKind::StringLiteral;
                case StringPrefix::L:
                    return TokenKind::WideStringLiteral;
                case StringPrefix::u8:
                    return TokenKind::UTF8StringLiteral;
                case StringPrefix::u:
                    return TokenKind::UTF16StringLiteral;
                case StringPrefix::U:
                    return TokenKind::UTF32StringLiteral;
                case StringPrefix::R:
                case StringPrefix::LR:
                case StringPrefix::u8R:
                case StringPrefix::uR:
                case StringPrefix::UR:
                    return TokenKind::RawStringLiteral;
                default:
                    return TokenKind::Invalid;
            }
        }

        bool TokenCreator::isValidPosition(size_t line, size_t column) {
            return line <= std::numeric_limits<unsigned int>::max() &&
                   column <= std::numeric_limits<unsigned int>::max();
        }

    }  // namespace frontend
}  // namespace rp
