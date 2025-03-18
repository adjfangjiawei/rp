#include "LexerTest.h"

#include <cassert>
#include <iomanip>
#include <iostream>
#include <memory>

#include "LexerCoverage.h"
#include "LexerTestUtils.h"

namespace rp {
    namespace frontend {
        namespace test {

            bool LexerTest::runAllTests() {
                coverage::LexerCoverage::initialize();
                bool success = true;

                std::cout << "\n====================================\n";
                std::cout << "Running Lexer Test Suite\n";
                std::cout << "====================================\n\n";

                // 运行数字字面量测试
                success &= runTestGroup("Number Literals",
                                        {testNumberLiterals,
                                         testDecimalNumbers,
                                         testHexNumbers,
                                         testBinaryNumbers,
                                         testOctalNumbers,
                                         testFloatingPointNumbers,
                                         testNumberSuffixes,
                                         testInvalidNumbers});

                // 运行Unicode测试
                success &= runTestGroup(
                    "Unicode Support",
                    {testUnicodeIdentifiers, testUnicodeStrings, testUnicodeComments, testUnicodeEscapeSequences});

                // 运行错误恢复测试
                success &= runTestGroup("Error Recovery",
                                        {testErrorRecovery, testBasicRecovery, testPanicMode, testContextualRecovery});

                // 运行字符串和字符字面量测试
                success &=
                    runTestGroup("String and Character Literals",
                                 {testStringLiterals, testCharacterLiterals, testRawStrings, testEscapeSequences});

                // 生成覆盖率报告
                coverage::LexerCoverage::generateReport("lexer_coverage.html");

                return success;
            }

            bool LexerTest::testUnicodeIdentifiers() {
                std::vector<TestCase> tests = {
                    {"Basic Unicode Identifier",
                     "变量名",
                     {TokenKind::Identifier},
                     [](const Token &t) { return t.text == "变量名"; }},
                    {"Mixed ASCII and Unicode",
                     "user名字",
                     {TokenKind::Identifier},
                     [](const Token &t) { return t.text == "user名字"; }},
                    {"Unicode Escape Sequence", "\\u4F60\\u597D", {TokenKind::Identifier}, [](const Token &t) {
                         return t.text == "你好";
                     }}};
                return runTestCases("Unicode Identifiers", tests);
            }

            bool LexerTest::testErrorRecovery() {
                std::vector<TestCase> tests = {
                    {"Unterminated String", "\"unclosed string\n", {TokenKind::Invalid}, nullptr},
                    {"Invalid Character", "@#$", {TokenKind::Invalid}, nullptr},
                    {"Recovery After Error",
                     "@#$ validIdentifier",
                     {TokenKind::Invalid, TokenKind::Identifier},
                     nullptr}};
                return runTestCases("Error Recovery", tests);
            }

            bool LexerTest::testStringLiterals() {
                std::vector<TestCase> tests = {{"Simple String",
                                                "\"Hello, World!\"",
                                                {TokenKind::StringLiteral},
                                                [](const Token &t) { return t.text == "Hello, World!"; }},
                                               {"String with Escapes",
                                                "\"Hello\\nWorld\"",
                                                {TokenKind::StringLiteral},
                                                [](const Token &t) { return t.text == "Hello\nWorld"; }},
                                               {"Empty String", "\"\"", {TokenKind::StringLiteral}, [](const Token &t) {
                                                    return t.text.empty();
                                                }}};
                return runTestCases("String Literals", tests);
            }

            bool LexerTest::runTestGroup(const std::string &groupName, const std::vector<TestFunction> &tests) {
                std::cout << "Running " << groupName << " tests...\n";
                std::cout << std::string(40, '-') << "\n";

                bool groupSuccess = true;
                for (const auto &test : tests) {
                    groupSuccess &= test();
                }

                std::cout << std::string(40, '-') << "\n";
                std::cout << groupName << " tests: " << (groupSuccess ? "PASSED" : "FAILED") << "\n\n";
                return groupSuccess;
            }

            bool LexerTest::testNumberLiterals() {
                std::vector<TestCase> tests = {
                    {"Simple Integer",
                     "42",
                     {TokenKind::NumberLiteral},
                     [](const Token &t) { return t.intValue == 42; }},
                    {"Decimal Number",
                     "123.456",
                     {TokenKind::NumberLiteral},
                     [](const Token &t) { return t.floatValue == 123.456; }},
                    {"Hex Number",
                     "0xFF",
                     {TokenKind::NumberLiteral},
                     [](const Token &t) { return t.intValue == 255; }},
                    {"Binary Number",
                     "0b1010",
                     {TokenKind::NumberLiteral},
                     [](const Token &t) { return t.intValue == 10; }},
                    {"Number with Separator", "1'000'000", {TokenKind::NumberLiteral}, [](const Token &t) {
                         return t.intValue == 1000000;
                     }}};

                return runTestCases("Number Literals", tests);
            }

            std::vector<Token> LexerTest::tokenize(const std::string &input) {
                Lexer lexer;
                lexer.setSource(input.c_str(), input.length(), "test.rp");

                std::vector<Token> tokens;
                Token token;
                do {
                    token = lexer.nextToken();
                    tokens.push_back(std::move(token));
                    coverage::LexerCoverage::recordExecution("test.rp", token.line);
                } while (token.kind != TokenKind::EndOfFile);

                return tokens;
            }

            bool LexerTest::runTest(const TestCase &test) {
                std::cout << std::left << std::setw(50) << ("  " + test.name + "...");

                auto tokens = tokenize(test.input);

                // 检查token序列
                if (!compareTokens(tokens, test.expectedTokens)) {
                    std::cout << "FAILED (incorrect token sequence)\n";
                    reportTestFailure(test.name, "Token sequence mismatch");
                    return false;
                }

                // 如果有自定义验证器，运行它
                if (test.validator) {
                    for (const auto &token : tokens) {
                        if (!test.validator(token)) {
                            std::cout << "FAILED (validator)\n";
                            reportTestFailure(test.name, "Validator check failed");
                            return false;
                        }
                    }
                }

                std::cout << "PASSED\n";
                return true;
            }

            bool LexerTest::runTestCases(const std::string &groupName, const std::vector<TestCase> &tests) {
                bool success = true;
                for (const auto &test : tests) {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::compareTokens(const std::vector<Token> &actual, const std::vector<TokenKind> &expected) {
                if (actual.size() - 1 != expected.size()) {  // -1 因为实际tokens包含EOF
                    return false;
                }

                for (size_t i = 0; i < expected.size(); ++i) {
                    if (actual[i].kind != expected[i]) {
                        return false;
                    }
                }

                return true;
            }

            void LexerTest::reportTestFailure(const std::string &testName, const std::string &message) {
                std::cerr << "\nTest failed: " << testName << "\n";
                std::cerr << "Error: " << message << "\n";
                std::cerr << std::string(50, '-') << "\n";
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
