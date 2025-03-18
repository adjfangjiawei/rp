#include <iostream>

#include "LexerTest.h"
#include "LexerTestUtils.h"

namespace rp {
    namespace frontend {
        namespace test {

            bool LexerTest::testBasicRecovery() {
                std::vector<TestCase> tests = {{"Invalid Character Recovery",
                                                "let @ x = 42;",
                                                {TokenKind::Identifier,
                                                 TokenKind::Invalid,
                                                 TokenKind::Identifier,
                                                 TokenKind::Equal,
                                                 TokenKind::NumberLiteral,
                                                 TokenKind::Semicolon},
                                                nullptr},
                                               {"Multiple Error Recovery",
                                                "let # x @ y = 42;",
                                                {TokenKind::Identifier,
                                                 TokenKind::Invalid,
                                                 TokenKind::Identifier,
                                                 TokenKind::Invalid,
                                                 TokenKind::Identifier,
                                                 TokenKind::Equal,
                                                 TokenKind::NumberLiteral,
                                                 TokenKind::Semicolon},
                                                nullptr},
                                               {"Invalid Number Recovery",
                                                "let x = 123.4.5;",
                                                {TokenKind::Identifier,
                                                 TokenKind::Identifier,
                                                 TokenKind::Equal,
                                                 TokenKind::Invalid,
                                                 TokenKind::Semicolon},
                                                nullptr},
                                               {"Unterminated String Recovery",
                                                "let x = \"hello;\nlet y = 42;",
                                                {TokenKind::Identifier,
                                                 TokenKind::Identifier,
                                                 TokenKind::Equal,
                                                 TokenKind::Invalid,
                                                 TokenKind::Identifier,
                                                 TokenKind::Identifier,
                                                 TokenKind::Equal,
                                                 TokenKind::NumberLiteral,
                                                 TokenKind::Semicolon},
                                                nullptr}};

                bool success = true;
                for (const auto &test : tests) {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testPanicMode() {
                std::vector<TestCase> tests = {{"Multiple Syntax Errors",
                                                "let @ # $ % x = 42;",
                                                {TokenKind::Identifier,
                                                 TokenKind::Invalid,
                                                 TokenKind::Identifier,
                                                 TokenKind::Equal,
                                                 TokenKind::NumberLiteral,
                                                 TokenKind::Semicolon},
                                                nullptr},
                                               {"Nested Error Recovery",
                                                "class Test { let @ # $ foo() { return 42; } }",
                                                {TokenKind::Keyword_Class,
                                                 TokenKind::Identifier,
                                                 TokenKind::LBrace,
                                                 TokenKind::Identifier,
                                                 TokenKind::Invalid,
                                                 TokenKind::Identifier,
                                                 TokenKind::LParen,
                                                 TokenKind::RParen,
                                                 TokenKind::LBrace,
                                                 TokenKind::Keyword_Return,
                                                 TokenKind::NumberLiteral,
                                                 TokenKind::Semicolon,
                                                 TokenKind::RBrace,
                                                 TokenKind::RBrace},
                                                nullptr}};

                bool success = true;
                for (const auto &test : tests) {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testContextualRecovery() {
                std::vector<TestCase> tests = {
                    {"Recovery in Expression",
                     "let x = (1 + @ 2) * 3;",
                     {TokenKind::Identifier,
                      TokenKind::Identifier,
                      TokenKind::Equal,
                      TokenKind::LParen,
                      TokenKind::NumberLiteral,
                      TokenKind::Plus,
                      TokenKind::Invalid,
                      TokenKind::NumberLiteral,
                      TokenKind::RParen,
                      TokenKind::Star,
                      TokenKind::NumberLiteral,
                      TokenKind::Semicolon},
                     nullptr},
                    {"Recovery in Function Declaration",
                     "fn test(x: @, y: int) -> int { return x + y; }",
                     {TokenKind::Identifier, TokenKind::Identifier, TokenKind::LParen,         TokenKind::Identifier,
                      TokenKind::Colon,      TokenKind::Invalid,    TokenKind::Comma,          TokenKind::Identifier,
                      TokenKind::Colon,      TokenKind::Identifier, TokenKind::RParen,         TokenKind::Arrow,
                      TokenKind::Identifier, TokenKind::LBrace,     TokenKind::Keyword_Return, TokenKind::Identifier,
                      TokenKind::Plus,       TokenKind::Identifier, TokenKind::Semicolon,      TokenKind::RBrace},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests) {
                    success &= runTest(test);
                }
                return success;
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
