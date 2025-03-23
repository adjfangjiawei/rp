#include "TokenUtils.h"

#include <array>
#include <unordered_map>

namespace rp {
    namespace frontend {
        namespace TokenUtils {
            namespace {
                // Token类型查找表的大小
                constexpr size_t TOKEN_MAP_SIZE = 1024;

                // 使用数组实现快速查找表
                template <size_t N>
                class TokenMap {
                  private:
                    std::array<bool, N> map{};

                  public:
                    void set(TokenKind kind, bool value = true) {
                        size_t index = static_cast<size_t>(kind);
                        if (index < N) {
                            map[index] = value;
                        }
                    }

                    bool get(TokenKind kind) const {
                        size_t index = static_cast<size_t>(kind);
                        return index < N ? map[index] : false;
                    }
                };

                // 初始化各种Token类型的查找表
                const auto initStringLiteralMap = []() {
                    TokenMap<TOKEN_MAP_SIZE> map;
                    map.set(TokenKind::StringLiteral);
                    map.set(TokenKind::RawStringLiteral);
                    map.set(TokenKind::WideStringLiteral);
                    map.set(TokenKind::UTF8StringLiteral);
                    map.set(TokenKind::UTF16StringLiteral);
                    map.set(TokenKind::UTF32StringLiteral);
                    return map;
                }();

                const auto initErrorTokenMap = []() {
                    TokenMap<TOKEN_MAP_SIZE> map;
                    map.set(TokenKind::Invalid);
                    map.set(TokenKind::StringLiteral_Unterminated);
                    map.set(TokenKind::StringLiteral_InvalidEscape);
                    map.set(TokenKind::StringLiteral_InvalidUTF8);
                    map.set(TokenKind::StringLiteral_InvalidDelimiter);
                    return map;
                }();

                const auto initBasicTypeMap = []() {
                    TokenMap<TOKEN_MAP_SIZE> map;
                    map.set(TokenKind::Keyword_Void);
                    map.set(TokenKind::Keyword_Bool);
                    map.set(TokenKind::Keyword_Char);
                    map.set(TokenKind::Keyword_Short);
                    map.set(TokenKind::Keyword_Int);
                    map.set(TokenKind::Keyword_Long);
                    map.set(TokenKind::Keyword_Float);
                    map.set(TokenKind::Keyword_Double);
                    return map;
                }();

                const auto initCharTypeMap = []() {
                    TokenMap<TOKEN_MAP_SIZE> map;
                    map.set(TokenKind::Keyword_Char);
                    map.set(TokenKind::Keyword_Char8_t);
                    map.set(TokenKind::Keyword_Char16_t);
                    map.set(TokenKind::Keyword_Char32_t);
                    return map;
                }();

                const auto initTypeModifierMap = []() {
                    TokenMap<TOKEN_MAP_SIZE> map;
                    map.set(TokenKind::Keyword_Signed);
                    map.set(TokenKind::Keyword_Unsigned);
                    map.set(TokenKind::Keyword_Long);
                    map.set(TokenKind::Keyword_Short);
                    return map;
                }();

                const auto initStorageClassMap = []() {
                    TokenMap<TOKEN_MAP_SIZE> map;
                    map.set(TokenKind::Keyword_Static);
                    map.set(TokenKind::Keyword_Extern);
                    map.set(TokenKind::Keyword_Register);
                    map.set(TokenKind::Keyword_Auto);
                    map.set(TokenKind::Keyword_Mutable);
                    map.set(TokenKind::Keyword_Thread_local);
                    return map;
                }();

                const auto initFunctionSpecifierMap = []() {
                    TokenMap<TOKEN_MAP_SIZE> map;
                    map.set(TokenKind::Keyword_Inline);
                    map.set(TokenKind::Keyword_Virtual);
                    map.set(TokenKind::Keyword_Explicit);
                    map.set(TokenKind::Keyword_Constexpr);
                    return map;
                }();
            }  // namespace

            // 基础Token类型判断实现
            bool isBaseToken(TokenKind kind) {
                return kind > TokenKind::FirstBaseToken && kind < TokenKind::LastBaseToken;
            }

            bool isStringLiteralToken(TokenKind kind) {
                return kind > TokenKind::FirstStringLiteral && kind < TokenKind::LastStringLiteral;
            }

            bool isKeyword(TokenKind kind) { return kind > TokenKind::FirstKeyword && kind < TokenKind::LastKeyword; }

            bool isOperator(TokenKind kind) {
                return kind > TokenKind::FirstOperator && kind < TokenKind::LastOperator;
            }

            bool isDelimiter(TokenKind kind) {
                return kind > TokenKind::FirstDelimiter && kind < TokenKind::LastDelimiter;
            }

            bool isDirective(TokenKind kind) {
                return kind > TokenKind::FirstDirective && kind < TokenKind::LastDirective;
            }

            // 字符串字面量相关函数实现
            bool isStringLiteral(TokenKind kind) { return initStringLiteralMap.get(kind); }

            bool isRawStringLiteral(TokenKind kind) { return kind == TokenKind::RawStringLiteral; }

            bool isWideStringLiteral(TokenKind kind) { return kind == TokenKind::WideStringLiteral; }

            bool isUTF8StringLiteral(TokenKind kind) { return kind == TokenKind::UTF8StringLiteral; }

            bool isUTF16StringLiteral(TokenKind kind) { return kind == TokenKind::UTF16StringLiteral; }

            bool isUTF32StringLiteral(TokenKind kind) { return kind == TokenKind::UTF32StringLiteral; }

            bool isStringPrefix(TokenKind kind) {
                return kind >= TokenKind::StringPrefix_L && kind <= TokenKind::StringPrefix_R;
            }

            bool isStringError(TokenKind kind) { return initErrorTokenMap.get(kind); }

            bool isValidStringPrefix(const std::string& prefix) {
                return prefix == "L" || prefix == "u8" || prefix == "u" || prefix == "U" || prefix == "R";
            }

            TokenKind getStringPrefixKind(const std::string& prefix) {
                if (prefix == "L") return TokenKind::StringPrefix_L;
                if (prefix == "u8") return TokenKind::StringPrefix_u8;
                if (prefix == "u") return TokenKind::StringPrefix_u;
                if (prefix == "U") return TokenKind::StringPrefix_U;
                if (prefix == "R") return TokenKind::StringPrefix_R;
                return TokenKind::Invalid;
            }

            TokenKind getStringLiteralKind(const std::string& prefix) {
                if (prefix == "L") return TokenKind::WideStringLiteral;
                if (prefix == "u8") return TokenKind::UTF8StringLiteral;
                if (prefix == "u") return TokenKind::UTF16StringLiteral;
                if (prefix == "U") return TokenKind::UTF32StringLiteral;
                if (prefix == "R") return TokenKind::RawStringLiteral;
                return TokenKind::StringLiteral;
            }

            // 类型系统相关函数实现
            bool isBasicTypeSpecifier(TokenKind kind) { return initBasicTypeMap.get(kind); }

            bool isCharTypeSpecifier(TokenKind kind) { return initCharTypeMap.get(kind); }

            bool isTypeModifier(TokenKind kind) { return initTypeModifierMap.get(kind); }

            bool isTypeSpecifier(TokenKind kind) {
                return isBasicTypeSpecifier(kind) || isCharTypeSpecifier(kind) || isTypeModifier(kind) ||
                       kind == TokenKind::Keyword_Auto;
            }

            bool isStorageClassSpecifier(TokenKind kind) { return initStorageClassMap.get(kind); }

            bool isDeclarationSpecifier(TokenKind kind) {
                return isTypeSpecifier(kind) || isStorageClassSpecifier(kind) || isFunctionSpecifier(kind);
            }

            // 运算符相关函数实现
            bool isAssignmentOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Equal:
                    case TokenKind::PlusEqual:
                    case TokenKind::MinusEqual:
                    case TokenKind::StarEqual:
                    case TokenKind::SlashEqual:
                    case TokenKind::PercentEqual:
                    case TokenKind::AmpEqual:
                    case TokenKind::PipeEqual:
                    case TokenKind::CaretEqual:
                        return true;
                    default:
                        return false;
                }
            }

            bool isArithmeticOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Plus:
                    case TokenKind::Minus:
                    case TokenKind::Star:
                    case TokenKind::Slash:
                    case TokenKind::Percent:
                        return true;
                    default:
                        return false;
                }
            }

            bool isBitwiseOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Ampersand:
                    case TokenKind::Pipe:
                    case TokenKind::Caret:
                    case TokenKind::Tilde:
                    case TokenKind::LessLess:
                    case TokenKind::GreaterGreater:
                        return true;
                    default:
                        return false;
                }
            }

            bool isLogicalOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::AmpAmp:
                    case TokenKind::PipePipe:
                    case TokenKind::Exclaim:
                        return true;
                    default:
                        return false;
                }
            }

            bool isComparisonOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::EqualEqual:
                    case TokenKind::ExclaimEqual:
                    case TokenKind::Less:
                    case TokenKind::Greater:
                    case TokenKind::LessEqual:
                    case TokenKind::GreaterEqual:
                    case TokenKind::Spaceship:
                        return true;
                    default:
                        return false;
                }
            }

            bool isUnaryOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Plus:
                    case TokenKind::Minus:
                    case TokenKind::Star:
                    case TokenKind::Ampersand:
                    case TokenKind::Exclaim:
                    case TokenKind::Tilde:
                        return true;
                    default:
                        return false;
                }
            }

            bool isIncrementDecrementOperator(TokenKind kind) {
                return kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
            }

            // 错误处理相关函数实现
            bool isErrorToken(TokenKind kind) { return initErrorTokenMap.get(kind); }

            const char* getErrorMessage(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Invalid:
                        return "Invalid token";
                    case TokenKind::StringLiteral_Unterminated:
                        return "Unterminated string literal";
                    case TokenKind::StringLiteral_InvalidEscape:
                        return "Invalid escape sequence in string literal";
                    case TokenKind::StringLiteral_InvalidUTF8:
                        return "Invalid UTF-8 sequence in string literal";
                    case TokenKind::StringLiteral_InvalidDelimiter:
                        return "Invalid delimiter in raw string literal";
                    default:
                        return "Unknown error";
                }
            }

            const char* getErrorDetail(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Invalid:
                        return "Token could not be recognized";
                    case TokenKind::StringLiteral_Unterminated:
                        return "String literal is missing closing quote";
                    case TokenKind::StringLiteral_InvalidEscape:
                        return "String contains an invalid escape sequence";
                    case TokenKind::StringLiteral_InvalidUTF8:
                        return "String contains invalid UTF-8 characters";
                    case TokenKind::StringLiteral_InvalidDelimiter:
                        return "Raw string literal has invalid delimiter";
                    default:
                        return "No detailed error information available";
                }
            }

            // Token分类实现
            TokenCategory getCategory(TokenKind kind) {
                if (isBaseToken(kind)) {
                    if (kind == TokenKind::EndOfFile || kind == TokenKind::Invalid) {
                        return TokenCategory::Special;
                    }
                    if (kind == TokenKind::Identifier) {
                        return TokenCategory::Identifier;
                    }
                    if (kind == TokenKind::NumberLiteral || kind == TokenKind::CharLiteral) {
                        return TokenCategory::Literal;
                    }
                }

                if (isStringLiteralToken(kind)) {
                    return TokenCategory::Literal;
                }

                if (isKeyword(kind)) {
                    return TokenCategory::Keyword;
                }

                if (isOperator(kind)) {
                    return TokenCategory::Operator;
                }

                if (isDelimiter(kind)) {
                    return TokenCategory::Delimiter;
                }

                return TokenCategory::Special;
            }

            // 其他实用函数实现
            bool isStatementTerminator(TokenKind kind) {
                return kind == TokenKind::Semicolon || kind == TokenKind::RBrace;
            }

            bool isCompoundStatementToken(TokenKind kind) {
                return kind == TokenKind::LBrace || kind == TokenKind::RBrace;
            }

            bool isAccessSpecifier(TokenKind kind) {
                return kind == TokenKind::Keyword_Public || kind == TokenKind::Keyword_Private ||
                       kind == TokenKind::Keyword_Protected;
            }

            bool isClassSpecifier(TokenKind kind) {
                return kind == TokenKind::Keyword_Class || kind == TokenKind::Keyword_Struct ||
                       kind == TokenKind::Keyword_Union;
            }

            bool isMemberAccessToken(TokenKind kind) { return kind == TokenKind::Period || kind == TokenKind::Arrow; }

            bool isTemplateToken(TokenKind kind) {
                return kind == TokenKind::Keyword_Template || kind == TokenKind::Less || kind == TokenKind::Greater;
            }

            bool isConstraintToken(TokenKind kind) {
                return kind == TokenKind::Keyword_Requires || kind == TokenKind::Keyword_Concept;
            }

            bool isLambdaToken(TokenKind kind) {
                return kind == TokenKind::LBrace;  // Lambda表达式以{开始
            }

            bool isCoroutineToken(TokenKind kind) {
                return kind == TokenKind::Keyword_co_await || kind == TokenKind::Keyword_co_yield ||
                       kind == TokenKind::Keyword_co_return;
            }

            bool isModuleToken(TokenKind kind) {
                return kind == TokenKind::Keyword_Module || kind == TokenKind::Keyword_Import;
            }

            bool isAttributeSpecifier(TokenKind kind) {
                return kind == TokenKind::LBrace && kind == TokenKind::LBrace;  // {{属性}}
            }

            bool isConstantSpecifier(TokenKind kind) {
                return kind == TokenKind::Keyword_Const || kind == TokenKind::Keyword_Constexpr ||
                       kind == TokenKind::Keyword_Consteval || kind == TokenKind::Keyword_Constinit;
            }

            bool isFunctionSpecifier(TokenKind kind) { return initFunctionSpecifierMap.get(kind); }

            bool isCpp11Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Auto:
                    case TokenKind::Keyword_Constexpr:
                    case TokenKind::Keyword_Decltype:
                    case TokenKind::Keyword_Nullptr:
                    case TokenKind::Keyword_Override:
                    case TokenKind::Keyword_Final:
                        return true;
                    default:
                        return false;
                }
            }

            bool isCpp14Keyword(TokenKind kind) { return kind == TokenKind::Keyword_Deprecated; }

            bool isCpp17Keyword(TokenKind kind) {
                return kind == TokenKind::Keyword_Inline || kind == TokenKind::Keyword_Fallthrough;
            }

            bool isCpp20Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Consteval:
                    case TokenKind::Keyword_Constinit:
                    case TokenKind::Keyword_co_await:
                    case TokenKind::Keyword_co_yield:
                    case TokenKind::Keyword_co_return:
                    case TokenKind::Keyword_Requires:
                        return true;
                    default:
                        return false;
                }
            }

            bool isExpressionStart(TokenKind kind) {
                return isUnaryOperator(kind) || kind == TokenKind::Identifier || kind == TokenKind::NumberLiteral ||
                       kind == TokenKind::StringLiteral || kind == TokenKind::CharLiteral || kind == TokenKind::LBrace;
            }

            bool isInitializationToken(TokenKind kind) { return kind == TokenKind::Equal || kind == TokenKind::LBrace; }

            const char* getTokenKindName(TokenKind kind) {
                switch (kind) {
#define TOKEN(name, str)  \
    case TokenKind::name: \
        return str;
#define KEYWORD(name, str)          \
    case TokenKind::Keyword_##name: \
        return str;
#define OPERATOR(name, str) \
    case TokenKind::name:   \
        return str;
#define DELIMITER(name, str) \
    case TokenKind::name:    \
        return str;
#define DIRECTIVE(name, str)          \
    case TokenKind::Directive_##name: \
        return str;
#define TYPE(name, str)             \
    case TokenKind::Keyword_##name: \
        return str;
#include "TokenKind.def"
#undef TOKEN
#undef KEYWORD
#undef OPERATOR
#undef DELIMITER
#undef DIRECTIVE
#undef TYPE
                    default:
                        return "unknown";
                }
            }

            // 预处理指令相关函数实现
            bool isPreprocessorDirective(TokenKind kind) {
                return kind > TokenKind::FirstDirective && kind < TokenKind::LastDirective;
            }

            // 块结构相关函数实现
            bool isBlockStart(TokenKind kind) { return kind == TokenKind::LBrace; }

            bool isBlockEnd(TokenKind kind) { return kind == TokenKind::RBrace; }

            // 错误恢复相关函数实现
            bool isRecoveryToken(TokenKind kind) {
                return kind == TokenKind::Semicolon || kind == TokenKind::RBrace || kind == TokenKind::EndOfFile;
            }

        }  // namespace TokenUtils
    }  // namespace frontend
}  // namespace rp
