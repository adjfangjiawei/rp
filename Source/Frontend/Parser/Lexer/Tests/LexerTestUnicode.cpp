
#include "LexerTest.h"
#include <string>

namespace rp
{
    namespace frontend
    {
        namespace test
        {

            bool LexerTest::testUnicodeStrings()
            {
                std::vector<TestCase> tests = {
                    {"Chinese String",
                     "\"你好，世界！\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Japanese String",
                     "\"こんにちは、世界！\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Korean String",
                     "\"안녕하세요, 세계!\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Mixed Unicode String",
                     "\"Hello, 世界! Привет! 안녕!\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Unicode Escape Sequence",
                     "\"\\u4F60\\u597D\"", // "你好" in Unicode escape
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"String with Unicode Symbols",
                     "\"π≈3.14159, ∑(n)=n(n+1)/2\"",
                     {TokenKind::StringLiteral},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testUnicodeComments()
            {
                std::vector<TestCase> tests = {
                    {"Single Line Unicode Comment",
                     "// 这是一个注释\n42",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Multi Line Unicode Comment",
                     "/* 这是一个\n多行注释\n用Unicode字符 */\n42",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Mixed Unicode Comment",
                     "// Comment with 漢字 and हिंदी\n42",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Nested Unicode Comments",
                     "/* Outer comment /* 内部注释 */ still in comment */\n42",
                     {TokenKind::NumberLiteral},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testUnicodeEscapeSequences()
            {
                std::vector<TestCase> tests = {
                    {"Basic Unicode Escape",
                     "\"\\u0041\\u0042\\u0043\"", // "ABC"
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"CJK Unicode Escape",
                     "\"\\u4F60\\u597D\\u3053\\u3093\"", // "你好こん"
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Mixed Unicode Escape",
                     "\"Hello\\u3001World\\uFF01\"", // "Hello、World！"
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Invalid Unicode Escape",
                     "\"\\uGHIJ\"",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Incomplete Unicode Escape",
                     "\"\\u12\"",
                     {TokenKind::Invalid},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testCharacterLiterals()
            {
                std::vector<TestCase> tests = {
                    {"ASCII Character",
                     "'A'",
                     {TokenKind::CharLiteral},
                     [](const Token &t)
                     { return t.intValue == 'A'; }},
                    {"Unicode Character",
                     "'世'",
                     {TokenKind::CharLiteral},
                     nullptr},
                    {"Escaped Character",
                     "'\\n'",
                     {TokenKind::CharLiteral},
                     [](const Token &t)
                     { return t.intValue == '\n'; }},
                    {"Unicode Escape Character",
                     "'\\u4F60'", // '你'
                     {TokenKind::CharLiteral},
                     nullptr},
                    {"Invalid Character Literal",
                     "'ab'",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Unterminated Character",
                     "'a",
                     {TokenKind::Invalid},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testRawStrings()
            {
                std::vector<TestCase> tests = {
                    {"Basic Raw String",
                     "R\"(Hello\nWorld)\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Raw String with Unicode",
                     "R\"(你好\n世界)\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Raw String with Delimiters",
                     "R\"delim(Hello(World)delim)\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Raw String with Escapes",
                     "R\"(\\n\\t\\r)\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Invalid Raw String",
                     "R\"(unterminated",
                     {TokenKind::Invalid},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testEscapeSequences()
            {
                std::vector<TestCase> tests = {
                    {"Common Escapes",
                     "\"\\n\\t\\r\\\"\\\\\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Hex Escapes",
                     "\"\\x41\\x42\\x43\"", // "ABC"
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Octal Escapes",
                     "\"\\101\\102\\103\"", // "ABC"
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Mixed Escapes",
                     "\"\\n\\x41\\u0042\\103\"",
                     {TokenKind::StringLiteral},
                     nullptr},
                    {"Invalid Escape",
                     "\"\\z\"",
                     {TokenKind::Invalid},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

        } // namespace test
    } // namespace frontend
} // namespace rp
