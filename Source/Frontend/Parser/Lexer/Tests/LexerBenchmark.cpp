#include "LexerBenchmark.h"

#include <chrono>
#include <iomanip>
#include <iostream>

#include "LexerTestUtils.h"

namespace rp {
    namespace frontend {
        namespace test {

            void LexerBenchmark::BenchmarkResult::print() const {
                std::cout << "\n=== " << name << " ===\n"
                          << "Input size: " << inputSize << " bytes\n"
                          << "Tokens processed: " << tokenCount << "\n"
                          << "Time taken: " << timeInMilliseconds << "ms\n"
                          << "Tokens per second: " << std::fixed << std::setprecision(2) << tokensPerSecond << "\n";
            }

            LexerBenchmark::BenchmarkResult LexerBenchmark::runBenchmark(const std::string& input,
                                                                         const std::string& name) {
                BenchmarkResult result;
                result.name = name;
                result.inputSize = input.length();

                auto start = std::chrono::high_resolution_clock::now();

                Lexer lexer;
                lexer.setSource(input.c_str(), input.length(), "benchmark.rp");

                // 处理所有token
                size_t tokenCount = 0;
                Token token;
                do {
                    token = lexer.nextToken();
                    tokenCount++;
                } while (token.kind != TokenKind::EndOfFile);

                auto end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

                result.tokenCount = tokenCount;
                result.timeInMilliseconds = duration.count();
                result.tokensPerSecond = (tokenCount * 1000.0) / duration.count();

                return result;
            }

            LexerBenchmark::BenchmarkResult LexerBenchmark::runSmallInputBenchmark() {
                return runBenchmark(LexerTestUtils::generateRandomCode(1000), "Small Input Benchmark");
            }

            LexerBenchmark::BenchmarkResult LexerBenchmark::runMediumInputBenchmark() {
                return runBenchmark(LexerTestUtils::generateRandomCode(10000), "Medium Input Benchmark");
            }

            LexerBenchmark::BenchmarkResult LexerBenchmark::runLargeInputBenchmark() {
                return runBenchmark(LexerTestUtils::generateRandomCode(100000), "Large Input Benchmark");
            }

            LexerBenchmark::BenchmarkResult LexerBenchmark::runUnicodeBenchmark() {
                std::string input;
                for (int i = 0; i < 1000; ++i) {
                    input += "变量" + std::to_string(i) + " = " + std::to_string(i) + ";\n";
                }
                return runBenchmark(input, "Unicode Benchmark");
            }

            LexerBenchmark::BenchmarkResult LexerBenchmark::runErrorRecoveryBenchmark() {
                std::string input;
                for (int i = 0; i < 1000; ++i) {
                    input += "var" + std::to_string(i) + " @ # $ = " + std::to_string(i) + ";\n";
                }
                return runBenchmark(input, "Error Recovery Benchmark");
            }

            void LexerBenchmark::runAllBenchmarks() {
                std::cout << "\n====================================\n";
                std::cout << "Lexer Benchmark Results\n";
                std::cout << "====================================\n";

                runSmallInputBenchmark().print();
                runMediumInputBenchmark().print();
                runLargeInputBenchmark().print();
                runUnicodeBenchmark().print();
                runErrorRecoveryBenchmark().print();

                std::cout << "\n====================================\n";
            }

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
