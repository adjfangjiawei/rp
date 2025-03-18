#include <chrono>
#include <iomanip>
#include <iostream>

#include "LexerTest.h"
#include "LexerTestUtils.h"

namespace rp {
    namespace frontend {
        namespace test {

            namespace {
                void runPerformanceTest(const std::string &input, const std::string &testName) {
                    auto start = std::chrono::high_resolution_clock::now();

                    Lexer lexer;
                    lexer.setSource(input.c_str(), input.length(), "perf_test.rp");

                    size_t tokenCount = 0;
                    Token token;
                    do {
                        token = lexer.nextToken();
                        tokenCount++;
                    } while (token.kind != TokenKind::EndOfFile);

                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                    std::cout << "Performance Test: " << testName << "\n";
                    std::cout << "Input size: " << input.length() << " bytes\n";
                    std::cout << "Tokens processed: " << tokenCount << "\n";
                    std::cout << "Time taken: " << duration.count() << "ms\n";
                    std::cout << "Tokens per second: " << std::fixed << std::setprecision(2)
                              << (tokenCount * 1000.0) / duration.count() << "\n\n";
                }
            }  // namespace

            void LexerTest::runPerformanceTests() {
                // 小规模测试
                runPerformanceTest(LexerTestUtils::generateRandomCode(1000), "Small Input");

                // 中等规模测试
                runPerformanceTest(LexerTestUtils::generateRandomCode(10000), "Medium Input");

                // 大规模测试
                runPerformanceTest(LexerTestUtils::generateRandomCode(100000), "Large Input");

                // Unicode测试
                std::string unicodeInput;
                for (int i = 0; i < 1000; ++i) {
                    unicodeInput += "变量" + std::to_string(i) + " = " + std::to_string(i) + ";\n";
                }
                runPerformanceTest(unicodeInput, "Unicode Input");

                // 错误恢复测试
                std::string errorInput;
                for (int i = 0; i < 1000; ++i) {
                    errorInput += "var" + std::to_string(i) + " @ # $ = " + std::to_string(i) + ";\n";
                }
                runPerformanceTest(errorInput, "Error Recovery");
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
