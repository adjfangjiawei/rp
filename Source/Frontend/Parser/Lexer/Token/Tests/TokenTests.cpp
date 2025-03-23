#include <gtest/gtest.h>

#include "../Token.h"

namespace rp {
    namespace frontend {
        namespace test {

            class TokenTest : public ::testing::Test {
              protected:
                void SetUp() override {}
                void TearDown() override {}
            };

            // 基本属性测试
            TEST_F(TokenTest, BasicTokenProperties) {
                Token token(TokenKind::Identifier);
                EXPECT_EQ(token.kind, TokenKind::Identifier);
                EXPECT_EQ(token.line, 0u);
                EXPECT_EQ(token.column, 0u);
            }

            // C++11关键字测试
            TEST_F(TokenTest, Cpp11Keywords) {
                EXPECT_TRUE(TokenUtils::isCpp11Keyword(TokenKind::Keyword_Alignas));
                EXPECT_TRUE(TokenUtils::isCpp11Keyword(TokenKind::Keyword_Nullptr));
                EXPECT_FALSE(TokenUtils::isCpp11Keyword(TokenKind::Keyword_Class));
                EXPECT_FALSE(TokenUtils::isCpp11Keyword(TokenKind::Keyword_Concept));  // C++20
            }

            // C++14关键字测试
            TEST_F(TokenTest, Cpp14Keywords) {
                EXPECT_TRUE(TokenUtils::isCpp14Keyword(TokenKind::Keyword_Deprecated));
                EXPECT_FALSE(TokenUtils::isCpp14Keyword(TokenKind::Keyword_Nullptr));
            }

            // C++17关键字测试
            TEST_F(TokenTest, Cpp17Keywords) {
                EXPECT_TRUE(TokenUtils::isCpp17Keyword(TokenKind::Keyword_Fallthrough));
                EXPECT_TRUE(TokenUtils::isCpp17Keyword(TokenKind::Keyword_Nodiscard));
                EXPECT_TRUE(TokenUtils::isCpp17Keyword(TokenKind::Keyword_Maybe_unused));
                EXPECT_FALSE(TokenUtils::isCpp17Keyword(TokenKind::Keyword_Concept));
            }

            // C++20关键字测试
            TEST_F(TokenTest, Cpp20Keywords) {
                EXPECT_TRUE(TokenUtils::isCpp20Keyword(TokenKind::Keyword_Concept));
                EXPECT_TRUE(TokenUtils::isCpp20Keyword(TokenKind::Keyword_Consteval));
                EXPECT_TRUE(TokenUtils::isCpp20Keyword(TokenKind::Keyword_co_await));
                EXPECT_FALSE(TokenUtils::isCpp20Keyword(TokenKind::Keyword_Class));
            }

            // 编译器指令测试
            TEST_F(TokenTest, PreprocessorDirectives) {
                EXPECT_TRUE(TokenUtils::isPreprocessorDirective(TokenKind::Directive_Include));
                EXPECT_TRUE(TokenUtils::isPreprocessorDirective(TokenKind::Directive_Define));
                EXPECT_TRUE(TokenUtils::isPreprocessorDirective(TokenKind::Directive_Pragma));
                EXPECT_FALSE(TokenUtils::isPreprocessorDirective(TokenKind::Keyword_Class));
            }

            // 错误恢复机制测试
            TEST_F(TokenTest, ErrorRecovery) {
                // 测试语句终止符
                EXPECT_TRUE(TokenUtils::isStatementTerminator(TokenKind::Semicolon));
                EXPECT_FALSE(TokenUtils::isStatementTerminator(TokenKind::Comma));

                // 测试块开始和结束
                EXPECT_TRUE(TokenUtils::isBlockStart(TokenKind::LBrace));
                EXPECT_TRUE(TokenUtils::isBlockEnd(TokenKind::RBrace));

                // 测试恢复token
                EXPECT_TRUE(TokenUtils::isRecoveryToken(TokenKind::Semicolon));
                EXPECT_TRUE(TokenUtils::isRecoveryToken(TokenKind::LBrace));
                EXPECT_TRUE(TokenUtils::isRecoveryToken(TokenKind::RBrace));
                EXPECT_FALSE(TokenUtils::isRecoveryToken(TokenKind::Plus));
            }

            // 类型说明符测试
            TEST_F(TokenTest, TypeSpecifiers) {
                EXPECT_TRUE(TokenUtils::isTypeSpecifier(TokenKind::Keyword_Int));
                EXPECT_TRUE(TokenUtils::isTypeSpecifier(TokenKind::Keyword_Char8_t));
                EXPECT_TRUE(TokenUtils::isTypeSpecifier(TokenKind::Keyword_Double));
                EXPECT_FALSE(TokenUtils::isTypeSpecifier(TokenKind::Keyword_Return));
            }

            // 存储类说明符测试
            TEST_F(TokenTest, StorageClassSpecifiers) {
                EXPECT_TRUE(TokenUtils::isStorageClassSpecifier(TokenKind::Keyword_Static));
                EXPECT_TRUE(TokenUtils::isStorageClassSpecifier(TokenKind::Keyword_Extern));
                EXPECT_TRUE(TokenUtils::isStorageClassSpecifier(TokenKind::Keyword_Mutable));
                EXPECT_FALSE(TokenUtils::isStorageClassSpecifier(TokenKind::Keyword_Int));
            }

            // 运算符测试
            TEST_F(TokenTest, OperatorTests) {
                // 赋值运算符测试
                EXPECT_TRUE(TokenUtils::isAssignmentOperator(TokenKind::Equal));
                EXPECT_TRUE(TokenUtils::isAssignmentOperator(TokenKind::PlusEqual));
                EXPECT_FALSE(TokenUtils::isAssignmentOperator(TokenKind::Plus));

                // 比较运算符测试
                EXPECT_TRUE(TokenUtils::isComparisonOperator(TokenKind::EqualEqual));
                EXPECT_TRUE(TokenUtils::isComparisonOperator(TokenKind::Spaceship));
                EXPECT_FALSE(TokenUtils::isComparisonOperator(TokenKind::Plus));

                // 自增自减运算符测试
                EXPECT_TRUE(TokenUtils::isIncrementDecrementOperator(TokenKind::PlusPlus));
                EXPECT_TRUE(TokenUtils::isIncrementDecrementOperator(TokenKind::MinusMinus));
                EXPECT_FALSE(TokenUtils::isIncrementDecrementOperator(TokenKind::Plus));
            }

            // 模板相关测试
            TEST_F(TokenTest, TemplateTests) {
                EXPECT_TRUE(TokenUtils::isTemplateToken(TokenKind::Keyword_Template));
                EXPECT_TRUE(TokenUtils::isTemplateToken(TokenKind::LessLessLess));
                EXPECT_TRUE(TokenUtils::isTemplateToken(TokenKind::GreaterGreaterGreater));
                EXPECT_FALSE(TokenUtils::isTemplateToken(TokenKind::Less));
            }

            // 访问说明符测试
            TEST_F(TokenTest, AccessSpecifierTests) {
                EXPECT_TRUE(TokenUtils::isAccessSpecifier(TokenKind::Keyword_Public));
                EXPECT_TRUE(TokenUtils::isAccessSpecifier(TokenKind::Keyword_Private));
                EXPECT_TRUE(TokenUtils::isAccessSpecifier(TokenKind::Keyword_Protected));
                EXPECT_FALSE(TokenUtils::isAccessSpecifier(TokenKind::Keyword_Class));
            }

            // Token分类测试
            TEST_F(TokenTest, TokenCategories) {
                EXPECT_EQ(TokenUtils::getCategory(TokenKind::EndOfFile), TokenCategory::Special);
                EXPECT_EQ(TokenUtils::getCategory(TokenKind::NumberLiteral), TokenCategory::Literal);
                EXPECT_EQ(TokenUtils::getCategory(TokenKind::Identifier), TokenCategory::Identifier);
                EXPECT_EQ(TokenUtils::getCategory(TokenKind::Keyword_Class), TokenCategory::Keyword);
                EXPECT_EQ(TokenUtils::getCategory(TokenKind::Plus), TokenCategory::Operator);
                EXPECT_EQ(TokenUtils::getCategory(TokenKind::LParen), TokenCategory::Delimiter);
                EXPECT_EQ(TokenUtils::getCategory(TokenKind::LBrace), TokenCategory::Delimiter);
            }

            // 字符串字面量测试
            TEST_F(TokenTest, StringLiteralTests) {
                Token token(TokenKind::StringLiteral);
                token.setStringInfo(true, "delim");
                token.setStringEncoding(false, true, false, false);

                EXPECT_TRUE(token.isRawString());
                EXPECT_TRUE(token.isUTF8String());
                EXPECT_FALSE(token.isWideString());
                EXPECT_EQ(token.getDelimiter(), "delim");

                EXPECT_TRUE(TokenUtils::isStringLiteralToken(TokenKind::StringLiteral));
                EXPECT_TRUE(TokenUtils::isStringLiteralToken(TokenKind::RawStringLiteral));
                EXPECT_TRUE(TokenUtils::isStringLiteralToken(TokenKind::UTF8StringLiteral));
                EXPECT_FALSE(TokenUtils::isStringLiteralToken(TokenKind::CharLiteral));
            }

            // 错误处理和恢复测试
            TEST_F(TokenTest, ErrorHandlingAndRecovery) {
                Token token;
                token.setError("Test error", 5, 10);

                EXPECT_TRUE(token.hasError());
                EXPECT_EQ(token.getErrorMessage(), "Test error");

                auto errorLoc = token.getErrorLocation();
                EXPECT_TRUE(errorLoc.has_value());
                EXPECT_EQ(errorLoc->first, 5);
                EXPECT_EQ(errorLoc->second, 10);

                EXPECT_TRUE(TokenUtils::isErrorToken(TokenKind::Invalid));
                EXPECT_TRUE(TokenUtils::isErrorToken(TokenKind::StringLiteral_Unterminated));
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
