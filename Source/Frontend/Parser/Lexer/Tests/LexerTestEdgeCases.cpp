#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#include "LexerTest.h"
#include "LexerTestUtils.h"

namespace rp {
    namespace frontend {
        namespace test {

            namespace {
                // 测试报告类
                class TestReport {
                  public:
                    void addTestResult(const std::string &testName, bool passed, const std::string &details = "") {
                        results.push_back({testName, passed, details});
                    }

                    void printReport() const {
                        size_t totalTests = results.size();
                        size_t passedTests =
                            std::count_if(results.begin(), results.end(), [](const TestResult &r) { return r.passed; });

                        std::cout << "\n=== Lexer Test Report ===\n";
                        std::cout << "Total Tests: " << totalTests << "\n";
                        std::cout << "Passed: " << passedTests << "\n";
                        std::cout << "Failed: " << (totalTests - passedTests) << "\n";
                        std::cout << "Success Rate: " << std::fixed << std::setprecision(2)
                                  << (passedTests * 100.0 / totalTests) << "%\n\n";

                        std::cout << "Detailed Results:\n";
                        std::cout << std::setfill('-') << std::setw(80) << "-" << "\n";
                        for (const auto &result : results) {
                            std::cout << (result.passed ? "[PASS] " : "[FAIL] ") << result.testName << "\n";
                            if (!result.details.empty()) {
                                std::cout << "  Details: " << result.details << "\n";
                            }
                        }
                        std::cout << std::setfill('-') << std::setw(80) << "-" << "\n";
                    }

                  private:
                    struct TestResult {
                        std::string testName;
                        bool passed;
                        std::string details;
                    };
                    std::vector<TestResult> results;
                };
            }  // namespace

            void LexerTest::testEdgeCases() {
                TestReport report;

                // 空输入测试
                {
                    TestCase test{"Empty Input", "", {TokenKind::EndOfFile}, nullptr};
                    bool result = runTest(test);
                    report.addTestResult(test.name, result);
                }

                // 最大标识符长度测试
                {
                    std::string longIdentifier = LexerTestUtils::generateIdentifier(1024);
                    TestCase test{"Maximum Identifier Length", longIdentifier, {TokenKind::Identifier}, nullptr};
                    bool result = runTest(test);
                    report.addTestResult(test.name, result);
                }

                // 嵌套注释测试
                {
                    std::string nestedComments = LexerTestUtils::generateNestedComments(5);
                    TestCase test{"Nested Comments", nestedComments + "code", {TokenKind::Identifier}, nullptr};
                    bool result = runTest(test);
                    report.addTestResult(test.name, result);
                }

                // Unicode边界测试
                {
                    TestCase test{"Unicode Boundaries", "\"\\u0000\\uffff\"", {TokenKind::StringLiteral}, nullptr};
                    bool result = runTest(test);
                    report.addTestResult(test.name, result);
                }

                // 混合字面量测试
                {
                    TestCase test{"Mixed Literals",
                                  "42 0x2A 0b101010 \"*\" '*'",
                                  {TokenKind::NumberLiteral,
                                   TokenKind::NumberLiteral,
                                   TokenKind::NumberLiteral,
                                   TokenKind::StringLiteral,
                                   TokenKind::CharLiteral},
                                  nullptr};
                    bool result = runTest(test);
                    report.addTestResult(test.name, result);
                }

                report.printReport();
            }

            void LexerTest::testMemoryUsage() {
                auto startStats = LexerTestUtils::getMemoryStats();

                // 大规模输入测试
                std::string largeInput = LexerTestUtils::generateRandomCode(100000);
                Lexer lexer;
                lexer.setSource(largeInput.c_str(), largeInput.length(), "memory_test.rp");

                // 记录峰值内存使用
                auto peakStats = LexerTestUtils::getMemoryStats();

                // 处理所有token
                Token token;
                size_t tokenCount = 0;
                do {
                    token = lexer.nextToken();
                    tokenCount++;
                } while (token.kind != TokenKind::EndOfFile);

                // 最终内存统计
                auto endStats = LexerTestUtils::getMemoryStats();

                std::cout << "\n=== Memory Usage Report ===\n";
                std::cout << "Initial Memory: " << startStats.currentMemoryUsage << " bytes\n";
                std::cout << "Peak Memory: " << peakStats.peakMemoryUsage << " bytes\n";
                std::cout << "Final Memory: " << endStats.currentMemoryUsage << " bytes\n";
                std::cout << "Memory Overhead per Token: "
                          << (peakStats.peakMemoryUsage - startStats.currentMemoryUsage) / tokenCount << " bytes\n";
            }

            void LexerTest::testConcurrency() {
                const size_t threadCount = std::thread::hardware_concurrency();
                std::vector<std::future<bool>> futures;

                // 创建多个并发任务
                for (size_t i = 0; i < threadCount; ++i) {
                    futures.push_back(std::async(std::launch::async, [i]() {
                        std::string input = LexerTestUtils::generateRandomCode(10000);
                        Lexer lexer;
                        lexer.setSource(input.c_str(), input.length(), "concurrent_test_" + std::to_string(i) + ".rp");

                        Token token;
                        do {
                            token = lexer.nextToken();
                            if (token.kind == TokenKind::Invalid) {
                                return false;
                            }
                        } while (token.kind != TokenKind::EndOfFile);

                        return true;
                    }));
                }

                // 等待所有任务完成并检查结果
                bool success = true;
                for (size_t i = 0; i < threadCount; ++i) {
                    success &= futures[i].get();
                }

                std::cout << "\n=== Concurrency Test Report ===\n";
                std::cout << "Threads: " << threadCount << "\n";
                std::cout << "Result: " << (success ? "PASSED" : "FAILED") << "\n";
            }

            void LexerTest::generateTestReport() {
                std::cout << "\n====================================\n";
                std::cout << "Lexer Comprehensive Test Report\n";
                std::cout << "====================================\n\n";

                // 运行所有测试类别
                testEdgeCases();
                testMemoryUsage();
                testConcurrency();

                // 打印总结
                std::cout << "\n=== Test Summary ===\n";
                std::cout << "Test suite completed.\n";
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
