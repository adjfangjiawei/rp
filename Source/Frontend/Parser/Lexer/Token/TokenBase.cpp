#include "TokenBase.h"

#include <sstream>

#include "TokenUtils.h"

namespace rp {
    namespace frontend {

        // 构造函数实现
        Token::Token() : kind(TokenKind::Invalid), line(0), column(0), intValue(0) {}

        Token::Token(TokenKind k) : kind(k), line(0), column(0), intValue(0) {}

        Token::Token(TokenKind k, unsigned ln, unsigned col) : kind(k), line(ln), column(col), intValue(0) {}

        Token::Token(TokenKind k, size_t pos, unsigned ln, unsigned col)
            : kind(k), line(ln), column(col), endPos(pos), intValue(0) {}

        Token::Token(TokenKind k, unsigned ln, unsigned col, const std::string& fname)
            : kind(k), line(ln), column(col), filename(fname), intValue(0) {}

        // 拷贝构造函数
        Token::Token(const Token& other)
            : kind(other.kind),
              textStorage(other.textStorage),
              text(other.textStorage),
              line(other.line),
              column(other.column),
              filename(other.filename),
              endPos(other.endPos),
              intValue(other.intValue) {
            if (other.errorInfo) {
                errorInfo = std::make_shared<ErrorInfo>(*other.errorInfo);
            }
            if (other.stringInfo) {
                stringInfo = std::make_shared<StringInfo>(*other.stringInfo);
            }
        }

        // 拷贝赋值运算符
        Token& Token::operator=(const Token& other) {
            if (this != &other) {
                kind = other.kind;
                textStorage = other.textStorage;
                text = textStorage;
                line = other.line;
                column = other.column;
                filename = other.filename;
                endPos = other.endPos;
                intValue = other.intValue;

                if (other.errorInfo) {
                    errorInfo = std::make_shared<ErrorInfo>(*other.errorInfo);
                } else {
                    errorInfo.reset();
                }

                if (other.stringInfo) {
                    stringInfo = std::make_shared<StringInfo>(*other.stringInfo);
                } else {
                    stringInfo.reset();
                }
            }
            return *this;
        }

        // 移动构造函数
        Token::Token(Token&& other) noexcept
            : kind(other.kind),
              textStorage(std::move(other.textStorage)),
              text(textStorage),
              line(other.line),
              column(other.column),
              filename(std::move(other.filename)),
              endPos(other.endPos),
              intValue(other.intValue),
              errorInfo(std::move(other.errorInfo)),
              stringInfo(std::move(other.stringInfo)) {}

        // 移动赋值运算符
        Token& Token::operator=(Token&& other) noexcept {
            if (this != &other) {
                kind = other.kind;
                textStorage = std::move(other.textStorage);
                text = textStorage;
                line = other.line;
                column = other.column;
                filename = std::move(other.filename);
                endPos = other.endPos;
                intValue = other.intValue;
                errorInfo = std::move(other.errorInfo);
                stringInfo = std::move(other.stringInfo);
            }
            return *this;
        }

        // 文本操作
        void Token::setText(std::string_view sv) {
            textStorage = std::string(sv);
            text = textStorage;
        }

        void Token::setText(std::string&& str) {
            textStorage = std::move(str);
            text = textStorage;
        }

        void Token::setText(const std::string& str) {
            textStorage = str;
            text = textStorage;
        }

        std::string_view Token::getText() const { return text; }

        // 错误处理
        void Token::setError(const std::string& message, unsigned errorLine, unsigned errorColumn) {
            if (!errorInfo) {
                errorInfo = std::make_shared<ErrorInfo>();
            }
            errorInfo->hasError = true;
            errorInfo->message = message;
            errorInfo->errorLine = errorLine ? errorLine : line;
            errorInfo->errorColumn = errorColumn ? errorColumn : column;
        }

        bool Token::hasError() const { return errorInfo && errorInfo->hasError; }

        std::string Token::getErrorMessage() const { return errorInfo ? errorInfo->message : ""; }

        std::optional<std::pair<unsigned, unsigned>> Token::getErrorLocation() const {
            if (errorInfo && errorInfo->hasError) {
                return std::make_pair(errorInfo->errorLine, errorInfo->errorColumn);
            }
            return std::nullopt;
        }

        // 字符串信息操作
        void Token::setStringInfo(bool isRaw, const std::string& delim) {
            if (!stringInfo) {
                stringInfo = std::make_shared<StringInfo>();
            }
            stringInfo->isRawString = isRaw;
            stringInfo->delimiter = delim;
        }

        void Token::setStringEncoding(bool isWide, bool isUTF8, bool isUTF16, bool isUTF32) {
            if (!stringInfo) {
                stringInfo = std::make_shared<StringInfo>();
            }
            stringInfo->isWide = isWide;
            stringInfo->isUTF8 = isUTF8;
            stringInfo->isUTF16 = isUTF16;
            stringInfo->isUTF32 = isUTF32;
        }

        // 字符串信息获取
        bool Token::isRawString() const { return stringInfo && stringInfo->isRawString; }

        bool Token::isWideString() const { return stringInfo && stringInfo->isWide; }

        bool Token::isUTF8String() const { return stringInfo && stringInfo->isUTF8; }

        bool Token::isUTF16String() const { return stringInfo && stringInfo->isUTF16; }

        bool Token::isUTF32String() const { return stringInfo && stringInfo->isUTF32; }

        std::string Token::getDelimiter() const { return stringInfo ? stringInfo->delimiter : ""; }

        // 基本属性获取
        TokenKind Token::getKind() const { return kind; }

        unsigned Token::getLine() const { return line; }

        unsigned Token::getColumn() const { return column; }

        const std::string& Token::getFilename() const { return filename; }

        size_t Token::getEndPos() const { return endPos; }

        void Token::setEndPos(size_t pos) { endPos = pos; }

        // 位置信息
        std::string Token::getLocation() const {
            std::stringstream ss;
            if (!filename.empty()) {
                ss << filename << ":";
            }
            ss << line << ":" << column;
            return ss.str();
        }

        // 值获取函数
        std::optional<long long> Token::getIntValue() const {
            if (TokenUtils::isBaseToken(kind) && kind == TokenKind::NumberLiteral) {
                return intValue;
            }
            return std::nullopt;
        }

        std::optional<double> Token::getFloatValue() const {
            if (TokenUtils::isBaseToken(kind) && kind == TokenKind::NumberLiteral) {
                return floatValue;
            }
            return std::nullopt;
        }

        std::optional<bool> Token::getBoolValue() const {
            if (kind == TokenKind::Keyword_True || kind == TokenKind::Keyword_False) {
                return boolValue;
            }
            return std::nullopt;
        }

        // 比较操作符
        bool Token::operator==(const Token& other) const {
            return kind == other.kind && text == other.text && line == other.line && column == other.column;
        }

        bool Token::operator!=(const Token& other) const { return !(*this == other); }

        bool Token::isKeyword() const { return TokenUtils::isKeyword(kind); }

    }  // namespace frontend
}  // namespace rp
