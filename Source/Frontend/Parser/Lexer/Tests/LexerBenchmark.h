#pragma once
#include <string>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {
        namespace test {

            class LexerBenchmark {
              public:
                struct BenchmarkResult {
                    std::string name;
                    size_t inputSize;
                    size_t tokenCount;
                    double timeInMilliseconds;
                    double tokensPerSecond;

                    // 打印结果
                    void print() const;
                };

                // 运行基准测试
                static BenchmarkResult runBenchmark(const std::string& input, const std::string& name);

                // 预定义的基准测试
                static BenchmarkResult runSmallInputBenchmark();
                static BenchmarkResult runMediumInputBenchmark();
                static BenchmarkResult runLargeInputBenchmark();
                static BenchmarkResult runUnicodeBenchmark();
                static BenchmarkResult runErrorRecoveryBenchmark();

                // 运行所有基准测试并生成报告
                static void runAllBenchmarks();
            };

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
