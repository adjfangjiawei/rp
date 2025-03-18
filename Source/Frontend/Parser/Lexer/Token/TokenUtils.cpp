#include "TokenUtils.h"

#include <array>
#include <unordered_map>

namespace rp {
    namespace frontend {
        namespace TokenUtils {
            namespace {
                // Token类型查找表的大小
                constexpr size_t TOKEN_ARRAY_SIZE = 256;

                // 使用数组来优化查找性能
                constexpr std::array<bool, TOKEN_ARRAY_SIZE> initializeStringLiteralArray() {
                    std::array<bool, 256> arr = {false};
                    arr[static_cast<size_t>(TokenKind::StringLiteral)] = true;
                    arr[static_cast<size_t>(TokenKind::RawStringLiteral)] = true;
                    arr[static_cast<size_t>(TokenKind::WideStringLiteral)] = true;
                    arr[static_cast<size_t>(TokenKind::UTF8StringLiteral)] = true;
                    arr[static_cast<size_t>(TokenKind::UTF16StringLiteral)] = true;
                    arr[static_cast<size_t>(TokenKind::UTF32StringLiteral)] = true;
                    return arr;
                }

                // 初始化错误token查找表
                constexpr std::array<bool, TOKEN_ARRAY_SIZE> initializeErrorTokenArray() {
                    std::array<bool, TOKEN_ARRAY_SIZE> arr = {false};
                    arr[static_cast<size_t>(TokenKind::Invalid)] = true;
                    arr[static_cast<size_t>(TokenKind::StringLiteral_Unterminated)] = true;
                    arr[static_cast<size_t>(TokenKind::StringLiteral_InvalidEscape)] = true;
                    arr[static_cast<size_t>(TokenKind::StringLiteral_InvalidUTF8)] = true;
                    arr[static_cast<size_t>(TokenKind::StringLiteral_InvalidDelimiter)] = true;
                    return arr;
                }

                // 初始化语句终止符查找表
                constexpr std::array<bool, TOKEN_ARRAY_SIZE> initializeTerminatorArray() {
                    std::array<bool, TOKEN_ARRAY_SIZE> arr = {false};
                    arr[static_cast<size_t>(TokenKind::Semicolon)] = true;
                    arr[static_cast<size_t>(TokenKind::RBrace)] = true;
                    return arr;
                }

                const auto stringLiteralArray = initializeStringLiteralArray();
                const auto errorTokenArray = initializeErrorTokenArray();
                const auto terminatorArray = initializeTerminatorArray();
            }  // namespace

            bool isStringLiteral(TokenKind kind) {
                return static_cast<size_t>(kind) < stringLiteralArray.size() &&
                       stringLiteralArray[static_cast<size_t>(kind)];
            }

            bool isRawStringLiteral(TokenKind kind) { return kind == TokenKind::RawStringLiteral; }

            bool isWideStringLiteral(TokenKind kind) { return kind == TokenKind::WideStringLiteral; }

            bool isUTF8StringLiteral(TokenKind kind) { return kind == TokenKind::UTF8StringLiteral; }

            bool isUTF16StringLiteral(TokenKind kind) { return kind == TokenKind::UTF16StringLiteral; }

            bool isUTF32StringLiteral(TokenKind kind) { return kind == TokenKind::UTF32StringLiteral; }

            bool isStringPrefix(TokenKind kind) {
                switch (kind) {
                    case TokenKind::StringPrefix_L:
                    case TokenKind::StringPrefix_u8:
                    case TokenKind::StringPrefix_u:
                    case TokenKind::StringPrefix_U:
                    case TokenKind::StringPrefix_R:
                        return true;
                    default:
                        return false;
                }
            }

            bool isStringError(TokenKind kind) {
                switch (kind) {
                    case TokenKind::StringLiteral_Unterminated:
                    case TokenKind::StringLiteral_InvalidEscape:
                    case TokenKind::StringLiteral_InvalidUTF8:
                    case TokenKind::StringLiteral_InvalidDelimiter:
                        return true;
                    default:
                        return false;
                }
            }

            static const std::unordered_map<std::string, TokenKind> stringPrefixMap = {
                {"L", TokenKind::StringPrefix_L},
                {"u8", TokenKind::StringPrefix_u8},
                {"u", TokenKind::StringPrefix_u},
                {"U", TokenKind::StringPrefix_U},
                {"R", TokenKind::StringPrefix_R}};

            static const std::unordered_map<std::string, TokenKind> stringLiteralMap = {
                {"L", TokenKind::WideStringLiteral},
                {"u8", TokenKind::UTF8StringLiteral},
                {"u", TokenKind::UTF16StringLiteral},
                {"U", TokenKind::UTF32StringLiteral},
                {"R", TokenKind::RawStringLiteral},
                {"", TokenKind::StringLiteral}};

            bool isValidStringPrefix(const std::string& prefix) {
                return stringPrefixMap.find(prefix) != stringPrefixMap.end();
            }

            TokenKind getStringPrefixKind(const std::string& prefix) {
                auto it = stringPrefixMap.find(prefix);
                return it != stringPrefixMap.end() ? it->second : TokenKind::Invalid;
            }

            TokenKind getStringLiteralKind(const std::string& prefix) {
                auto it = stringLiteralMap.find(prefix);
                return it != stringLiteralMap.end() ? it->second : TokenKind::Invalid;
            }

            bool isErrorToken(TokenKind kind) {
                return static_cast<size_t>(kind) < errorTokenArray.size() && errorTokenArray[static_cast<size_t>(kind)];
            }

            // 检查是否是语句终止符（包括分号和右花括号）
            bool isStatementTerminator(TokenKind kind) {
                return static_cast<size_t>(kind) < terminatorArray.size() && terminatorArray[static_cast<size_t>(kind)];
            }

            // 检查是否是复合语句相关的token
            bool isCompoundStatementToken(TokenKind kind) {
                return kind == TokenKind::LBrace || kind == TokenKind::RBrace;
            }

            // 检查是否是声明说明符
            bool isDeclarationSpecifier(TokenKind kind) {
                return isTypeSpecifier(kind) || isStorageClassSpecifier(kind) || kind == TokenKind::Keyword_Const ||
                       kind == TokenKind::Keyword_Volatile || kind == TokenKind::Keyword_Constexpr ||
                       kind == TokenKind::Keyword_Consteval || kind == TokenKind::Keyword_Constinit;
            }

            // 错误消息结构
            struct ErrorMessageInfo {
                const char* message;
                const char* detail;
            };

            // 获取详细的错误信息
            ErrorMessageInfo getDetailedErrorMessage(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Invalid:
                        return {"Invalid token", "The token is not recognized by the lexer"};
                    case TokenKind::StringLiteral_Unterminated:
                        return {"Unterminated string literal", "String literal is missing closing quotation mark"};
                    case TokenKind::StringLiteral_InvalidEscape:
                        return {"Invalid escape sequence in string literal",
                                "The escape sequence is not recognized or is malformed"};
                    case TokenKind::StringLiteral_InvalidUTF8:
                        return {"Invalid UTF-8 sequence in string literal", "The UTF-8 encoding sequence is malformed"};
                    case TokenKind::StringLiteral_InvalidDelimiter:
                        return {"Invalid delimiter in raw string literal",
                                "Raw string delimiter contains invalid characters or is malformed"};
                    default:
                        return {"Unknown error", "An unspecified error occurred"};
                }
            }

            // 获取基本错误消息
            const char* getErrorMessage(TokenKind kind) { return getDetailedErrorMessage(kind).message; }

            // 获取错误详细信息
            const char* getErrorDetail(TokenKind kind) { return getDetailedErrorMessage(kind).detail; }

            // 检查是否是函数声明相关的token
            bool isFunctionSpecifier(TokenKind kind) {
                return kind == TokenKind::Keyword_Inline || kind == TokenKind::Keyword_Virtual ||
                       kind == TokenKind::Keyword_Explicit || kind == TokenKind::Keyword_Constexpr;
            }

            // 检查是否是属性说明符
            bool isAttributeSpecifier(TokenKind kind) {
                return kind == TokenKind::Keyword_Deprecated || kind == TokenKind::Keyword_Nodiscard ||
                       kind == TokenKind::Keyword_Maybe_unused;
            }

            namespace {
                // 使用constexpr数组优化查找性能
                constexpr std::array<bool, 256> initializeKeywordArray() {
                    std::array<bool, 256> arr = {false};
                    for (size_t i = static_cast<size_t>(TokenKind::Keyword_Auto);
                         i <= static_cast<size_t>(TokenKind::Keyword_Export);
                         ++i) {
                        arr[i] = true;
                    }
                    return arr;
                }

                constexpr std::array<bool, 256> initializeOperatorArray() {
                    std::array<bool, 256> arr = {false};
                    for (size_t i = static_cast<size_t>(TokenKind::Plus); i <= static_cast<size_t>(TokenKind::Arrow);
                         ++i) {
                        arr[i] = true;
                    }
                    return arr;
                }

                constexpr std::array<bool, 256> initializeDelimiterArray() {
                    std::array<bool, 256> arr = {false};
                    for (size_t i = static_cast<size_t>(TokenKind::LParen); i <= static_cast<size_t>(TokenKind::RBrace);
                         ++i) {
                        arr[i] = true;
                    }
                    return arr;
                }

                const auto keywordArray = initializeKeywordArray();
                const auto operatorArray = initializeOperatorArray();
                const auto delimiterArray = initializeDelimiterArray();
            }  // namespace

            bool isKeyword(TokenKind kind) {
                return static_cast<size_t>(kind) < keywordArray.size() && keywordArray[static_cast<size_t>(kind)];
            }

            bool isOperator(TokenKind kind) {
                return static_cast<size_t>(kind) < operatorArray.size() && operatorArray[static_cast<size_t>(kind)];
            }

            bool isDelimiter(TokenKind kind) {
                return static_cast<size_t>(kind) < delimiterArray.size() && delimiterArray[static_cast<size_t>(kind)];
            }

            bool isDirective(TokenKind kind) {
                return (static_cast<int>(kind) >= static_cast<int>(TokenKind::Directive_Include) &&
                        static_cast<int>(kind) <= static_cast<int>(TokenKind::Directive_Warning));
            }

            bool isTypeKeyword(TokenKind kind) {
                return (static_cast<int>(kind) >= static_cast<int>(TokenKind::Keyword_Void) &&
                        static_cast<int>(kind) <= static_cast<int>(TokenKind::Keyword_Bool));
            }

            bool isCpp11Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Alignas:
                    case TokenKind::Keyword_Alignof:
                    case TokenKind::Keyword_Char16_t:
                    case TokenKind::Keyword_Char32_t:
                    case TokenKind::Keyword_Constexpr:
                    case TokenKind::Keyword_Decltype:
                    case TokenKind::Keyword_Noexcept:
                    case TokenKind::Keyword_Nullptr:
                    case TokenKind::Keyword_Static_assert:
                    case TokenKind::Keyword_Thread_local:
                        return true;
                    default:
                        return false;
                }
            }

            bool isCpp14Keyword(TokenKind kind) { return kind == TokenKind::Keyword_Deprecated; }

            bool isCpp17Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Fallthrough:
                    case TokenKind::Keyword_Nodiscard:
                    case TokenKind::Keyword_Maybe_unused:
                        return true;
                    default:
                        return false;
                }
            }

            bool isCpp20Keyword(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Concept:
                    case TokenKind::Keyword_Consteval:
                    case TokenKind::Keyword_Constinit:
                    case TokenKind::Keyword_co_await:
                    case TokenKind::Keyword_co_return:
                    case TokenKind::Keyword_co_yield:
                    case TokenKind::Keyword_Requires:
                        return true;
                    default:
                        return false;
                }
            }

            bool isBlockStart(TokenKind kind) { return kind == TokenKind::LBrace; }

            bool isBlockEnd(TokenKind kind) { return kind == TokenKind::RBrace; }

            bool isRecoveryToken(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Semicolon:
                    case TokenKind::LBrace:
                    case TokenKind::RBrace:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是基本类型说明符
            bool isBasicTypeSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Void:
                    case TokenKind::Keyword_Bool:
                    case TokenKind::Keyword_Char:
                    case TokenKind::Keyword_Short:
                    case TokenKind::Keyword_Int:
                    case TokenKind::Keyword_Long:
                    case TokenKind::Keyword_Float:
                    case TokenKind::Keyword_Double:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是字符类型说明符
            bool isCharTypeSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Char:
                    case TokenKind::Keyword_Char8_t:
                    case TokenKind::Keyword_Char16_t:
                    case TokenKind::Keyword_Char32_t:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是类型修饰符
            bool isTypeModifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Signed:
                    case TokenKind::Keyword_Unsigned:
                    case TokenKind::Keyword_Long:
                    case TokenKind::Keyword_Short:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是任何类型说明符
            bool isTypeSpecifier(TokenKind kind) {
                return isBasicTypeSpecifier(kind) || isCharTypeSpecifier(kind) || isTypeModifier(kind) ||
                       kind == TokenKind::Keyword_Auto;
            }

            // 检查是否是协程相关的token
            bool isCoroutineToken(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_co_await:
                    case TokenKind::Keyword_co_return:
                    case TokenKind::Keyword_co_yield:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是存储类说明符
            bool isStorageClassSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Static:
                    case TokenKind::Keyword_Extern:
                    case TokenKind::Keyword_Thread_local:
                    case TokenKind::Keyword_Mutable:
                    case TokenKind::Keyword_Register:
                    case TokenKind::Keyword_Auto:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是初始化相关的token
            bool isInitializationToken(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Equal:   // = 初始化
                    case TokenKind::LBrace:  // {} 列表初始化
                    case TokenKind::LParen:  // () 直接初始化
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是常量说明符
            bool isConstantSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Const:
                    case TokenKind::Keyword_Constexpr:
                    case TokenKind::Keyword_Consteval:
                    case TokenKind::Keyword_Constinit:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是类定义相关的token
            bool isClassSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Class:
                    case TokenKind::Keyword_Struct:
                    case TokenKind::Keyword_Union:
                    case TokenKind::Keyword_Final:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是模块相关的token
            bool isModuleToken(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Module:
                    case TokenKind::Keyword_Import:
                    case TokenKind::Keyword_Export:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是赋值运算符
            bool isAssignmentOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Equal:           // =
                    case TokenKind::PlusEqual:       // +=
                    case TokenKind::MinusEqual:      // -=
                    case TokenKind::StarEqual:       // *=
                    case TokenKind::SlashEqual:      // /=
                    case TokenKind::PercentEqual:    // %=
                    case TokenKind::AmpEqual:        // &=
                    case TokenKind::PipeEqual:       // |=
                    case TokenKind::CaretEqual:      // ^=
                    case TokenKind::LessLess:        // <<=
                    case TokenKind::GreaterGreater:  // >>=
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是算术运算符
            bool isArithmeticOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Plus:     // +
                    case TokenKind::Minus:    // -
                    case TokenKind::Star:     // *
                    case TokenKind::Slash:    // /
                    case TokenKind::Percent:  // %
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是位运算符
            bool isBitwiseOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Ampersand:       // &
                    case TokenKind::Pipe:            // |
                    case TokenKind::Caret:           // ^
                    case TokenKind::Tilde:           // ~
                    case TokenKind::LessLess:        // <<
                    case TokenKind::GreaterGreater:  // >>
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是比较运算符
            bool isComparisonOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::EqualEqual:    // ==
                    case TokenKind::ExclaimEqual:  // !=
                    case TokenKind::Less:          // <
                    case TokenKind::Greater:       // >
                    case TokenKind::LessEqual:     // <=
                    case TokenKind::GreaterEqual:  // >=
                    case TokenKind::Spaceship:     // <=>
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是逻辑运算符
            bool isLogicalOperator(TokenKind kind) {
                switch (kind) {
                    case TokenKind::AmpAmp:    // &&
                    case TokenKind::PipePipe:  // ||
                    case TokenKind::Exclaim:   // !
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是任何类型的运算符
            bool isAnyOperator(TokenKind kind) {
                return isAssignmentOperator(kind) || isArithmeticOperator(kind) || isBitwiseOperator(kind) ||
                       isComparisonOperator(kind) || isLogicalOperator(kind) || isIncrementDecrementOperator(kind);
            }

            // 检查是否是复合属性说明符
            bool isComplexAttributeSpecifier(TokenKind kind) {
                return isAttributeSpecifier(kind) || isConstantSpecifier(kind) || kind == TokenKind::Keyword_Virtual ||
                       kind == TokenKind::Keyword_Override || kind == TokenKind::Keyword_Final;
            }

            bool isIncrementDecrementOperator(TokenKind kind) {
                return kind == TokenKind::PlusPlus || kind == TokenKind::MinusMinus;
            }

            // 检查是否是模板相关的token
            bool isTemplateToken(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Template:
                    case TokenKind::LessLessLess:
                    case TokenKind::GreaterGreaterGreater:
                    case TokenKind::Less:     // 模板参数列表开始
                    case TokenKind::Greater:  // 模板参数列表结束
                    case TokenKind::Keyword_Typename:
                    case TokenKind::Keyword_Class:
                    case TokenKind::Keyword_Concept:
                    case TokenKind::Keyword_Requires:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是类型约束相关的token
            bool isConstraintToken(TokenKind kind) {
                return kind == TokenKind::Keyword_Concept || kind == TokenKind::Keyword_Requires ||
                       kind == TokenKind::Keyword_Typename;
            }

            // 检查是否是类成员访问相关的token
            bool isMemberAccessToken(TokenKind kind) {
                return kind == TokenKind::Period || kind == TokenKind::Arrow || kind == TokenKind::ColonColon;
            }

            // 检查是否是lambda表达式相关的token
            bool isLambdaToken(TokenKind kind) {
                return kind == TokenKind::LambdaIntro || kind == TokenKind::LambdaArrow;
            }

            bool isAccessSpecifier(TokenKind kind) {
                switch (kind) {
                    case TokenKind::Keyword_Public:
                    case TokenKind::Keyword_Private:
                    case TokenKind::Keyword_Protected:
                        return true;
                    default:
                        return false;
                }
            }

            // 检查是否是声明开始的token
            bool isDeclarationStart(TokenKind kind) {
                return isTypeSpecifier(kind) || isStorageClassSpecifier(kind) || isConstantSpecifier(kind) ||
                       isAttributeSpecifier(kind) || isClassSpecifier(kind) || kind == TokenKind::Keyword_Template ||
                       kind == TokenKind::Keyword_Typename;
            }

            // 检查是否是一元运算符
            bool isUnaryOperator(TokenKind kind) {
                return kind == TokenKind::Plus || kind == TokenKind::Minus || kind == TokenKind::Star ||
                       kind == TokenKind::Ampersand || kind == TokenKind::Exclaim || kind == TokenKind::Tilde;
            }

            // 检查是否是表达式开始的token
            bool isExpressionStart(TokenKind kind) {
                return kind == TokenKind::Identifier || kind == TokenKind::NumberLiteral || isStringLiteral(kind) ||
                       kind == TokenKind::CharLiteral || kind == TokenKind::LParen || kind == TokenKind::LBrace ||
                       isUnaryOperator(kind) || isIncrementDecrementOperator(kind);
            }

            // 获取Token的分类
            TokenCategory getCategory(TokenKind kind) {
                if (kind == TokenKind::EndOfFile || kind == TokenKind::Invalid) {
                    return TokenCategory::Special;
                }
                if (kind == TokenKind::NumberLiteral || isStringLiteral(kind) || kind == TokenKind::CharLiteral) {
                    return TokenCategory::Literal;
                }
                if (kind == TokenKind::Identifier) {
                    return TokenCategory::Identifier;
                }
                if (isKeyword(kind)) {
                    return TokenCategory::Keyword;
                }
                if (isAnyOperator(kind)) {
                    return TokenCategory::Operator;
                }
                if (isDelimiter(kind)) {
                    return TokenCategory::Delimiter;
                }
                if (isAttributeSpecifier(kind) || isComplexAttributeSpecifier(kind)) {
                    return TokenCategory::Attribute;
                }
                return TokenCategory::Special;
            }

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

        }  // namespace TokenUtils
    }  // namespace frontend
}  // namespace rp
