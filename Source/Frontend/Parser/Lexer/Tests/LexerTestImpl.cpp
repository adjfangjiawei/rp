
#include "LexerTest.h"
#include <limits>
#include <cmath>

namespace rp
{
    namespace frontend
    {
        namespace test
        {

            bool LexerTest::testDecimalNumbers()
            {
                std::vector<TestCase> tests = {
                    {"Simple Decimal",
                     "12345",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 12345; }},
                    {"Zero",
                     "0",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0; }},
                    {"Max Int",
                     "2147483647",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 2147483647; }},
                    {"Long Long Number",
                     "9223372036854775807",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == std::numeric_limits<long long>::max(); }},
                    {"Number with Underscores",
                     "1_234_567",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 1234567; }}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testHexNumbers()
            {
                std::vector<TestCase> tests = {
                    {"Simple Hex",
                     "0xFF",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 255; }},
                    {"Hex with Letters",
                     "0xABCDEF",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0xABCDEF; }},
                    {"Hex with Underscores",
                     "0xFF_FF_FF",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0xFFFFFF; }},
                    {"Zero Hex",
                     "0x0",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0; }},
                    {"Mixed Case Hex",
                     "0xaBcDeF",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0xABCDEF; }}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testBinaryNumbers()
            {
                std::vector<TestCase> tests = {
                    {"Simple Binary",
                     "0b1010",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 10; }},
                    {"Binary with Underscores",
                     "0b1010_1010",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 170; }},
                    {"Zero Binary",
                     "0b0",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0; }},
                    {"Long Binary",
                     "0b1111_1111_1111_1111",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 65535; }}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testOctalNumbers()
            {
                std::vector<TestCase> tests = {
                    {"Simple Octal",
                     "0755",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0755; }},
                    {"Octal with Underscores",
                     "0777_777",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0777777; }},
                    {"Zero Octal",
                     "00",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.intValue == 0; }}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testFloatingPointNumbers()
            {
                std::vector<TestCase> tests = {
                    {"Simple Float",
                     "123.456",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return std::abs(t.floatValue - 123.456) < 0.0001; }},
                    {"Scientific Notation",
                     "1.23e+4",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return std::abs(t.floatValue - 12300.0) < 0.0001; }},
                    {"Negative Exponent",
                     "1.23e-4",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return std::abs(t.floatValue - 0.000123) < 0.0000001; }},
                    {"Float with Underscores",
                     "1_234.567_89",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return std::abs(t.floatValue - 1234.56789) < 0.0001; }},
                    {"Zero Float",
                     "0.0",
                     {TokenKind::NumberLiteral},
                     [](const Token &t)
                     { return t.floatValue == 0.0; }}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testNumberSuffixes()
            {
                std::vector<TestCase> tests = {
                    {"Unsigned Suffix",
                     "42u",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Long Suffix",
                     "42l",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Long Long Suffix",
                     "42ll",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Unsigned Long Long",
                     "42ull",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Float Suffix",
                     "42.0f",
                     {TokenKind::NumberLiteral},
                     nullptr},
                    {"Double Suffix",
                     "42.0d",
                     {TokenKind::NumberLiteral},
                     nullptr}};

                bool success = true;
                for (const auto &test : tests)
                {
                    success &= runTest(test);
                }
                return success;
            }

            bool LexerTest::testInvalidNumbers()
            {
                std::vector<TestCase> tests = {
                    {"Multiple Decimal Points",
                     "123.456.789",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Invalid Hex Digit",
                     "0xGHIJ",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Invalid Binary Digit",
                     "0b102",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Invalid Octal Digit",
                     "0789",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Invalid Suffix Combination",
                     "42uf",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Incomplete Exponent",
                     "1.23e",
                     {TokenKind::Invalid},
                     nullptr},
                    {"Multiple Exponents",
                     "1.23e4e5",
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
