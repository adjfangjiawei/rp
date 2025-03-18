#pragma once
#include <functional>
#include <string>
#include <vector>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {
        namespace test {

            /**
             * @brief Lexer测试类
             *
             * 提供了一系列测试用例来验证Lexer的功能，包括：
             * - 基本词法分析功能
             * - 边界情况处理
             * - 错误恢复能力
             * - Unicode支持
             * - 内存使用情况
             * - 并发处理能力
             */
            class LexerTest {
              public:
                /**
                 * @brief 测试用例结构
                 */
                struct TestCase {
                    std::string name;                             ///< 测试用例名称
                    std::string input;                            ///< 输入文本
                    std::vector<TokenKind> expectedTokens;        ///< 期望的Token序列
                    std::function<bool(const Token&)> validator;  ///< 自定义验证函数
                };

                /**
                 * @brief 测试函数类型
                 */
                using TestFunction = std::function<bool()>;

                // 主要测试入口
                static bool runAllTests();         ///< 运行所有测试
                static void generateTestReport();  ///< 生成测试报告

                // 特殊测试类别
                static void testEdgeCases();        ///< 测试边界情况
                static void testMemoryUsage();      ///< 测试内存使用
                static void testConcurrency();      ///< 测试并发处理
                static void runPerformanceTests();  ///< 运行性能测试

              private:
                // 测试组织
                static bool runTestGroup(const std::string& groupName,
                                         const std::vector<TestFunction>& tests);  ///< 运行测试组
                static bool runTestCases(const std::string& groupName,
                                         const std::vector<TestCase>& tests);  ///< 运行测试用例组

                // 数字字面量测试
                static bool testNumberLiterals();        ///< 测试数字字面量
                static bool testDecimalNumbers();        ///< 测试十进制数
                static bool testHexNumbers();            ///< 测试十六进制数
                static bool testBinaryNumbers();         ///< 测试二进制数
                static bool testOctalNumbers();          ///< 测试八进制数
                static bool testFloatingPointNumbers();  ///< 测试浮点数
                static bool testNumberSuffixes();        ///< 测试数字后缀
                static bool testInvalidNumbers();        ///< 测试无效数字

                // Unicode测试
                static bool testUnicodeIdentifiers();      ///< 测试Unicode标识符
                static bool testUnicodeStrings();          ///< 测试Unicode字符串
                static bool testUnicodeComments();         ///< 测试Unicode注释
                static bool testUnicodeEscapeSequences();  ///< 测试Unicode转义序列

                // 错误恢复测试
                static bool testErrorRecovery();       ///< 测试错误恢复
                static bool testBasicRecovery();       ///< 测试基本恢复
                static bool testPanicMode();           ///< 测试恐慌模式
                static bool testContextualRecovery();  ///< 测试上下文相关恢复

                // 字符串和字符字面量测试
                static bool testStringLiterals();     ///< 测试字符串字面量
                static bool testCharacterLiterals();  ///< 测试字符字面量
                static bool testRawStrings();         ///< 测试原始字符串
                static bool testEscapeSequences();    ///< 测试转义序列

                // 辅助函数
                static bool runTest(const TestCase& test);  ///< 运行单个测试用例
                static bool compareTokens(const std::vector<Token>& actual,
                                          const std::vector<TokenKind>& expected);  ///< 比较Token序列
                static std::vector<Token> tokenize(const std::string& input);       ///< 对输入进行词法分析
                static void reportTestFailure(const std::string& testName,
                                              const std::string& message);  ///< 报告测试失败

                // 测试结果统计
                struct TestStatistics {
                    size_t totalTests{0};                      ///< 总测试数
                    size_t passedTests{0};                     ///< 通过的测试数
                    size_t failedTests{0};                     ///< 失败的测试数
                    std::vector<std::string> failedTestNames;  ///< 失败的测试名称
                };

                static TestStatistics stats;  ///< 测试统计信息
            };

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
