
#pragma once
#include "Lexer.h"
#include <string>
#include <vector>
#include <chrono>

namespace rp
{
    namespace frontend
    {
        namespace regression
        {

            // 性能回归测试
            class RegressionTest
            {
            public:
                // 性能基准
                struct Baseline
                {
                    std::string testName;
                    double expectedTime;
                    double tolerance;
                    size_t expectedMemory;
                    size_t memoryTolerance;
                };

                // 回归测试结果
                struct RegressionResult
                {
                    std::string testName;
                    bool passed;
                    double actualTime;
                    double expectedTime;
                    double timeDifference;
                    size_t actualMemory;
                    size_t expectedMemory;
                    size_t memoryDifference;
                    std::string failureReason;
                };

                // 运行回归测试
                static std::vector<RegressionResult> runRegressionTests();

                // 更新基准数据
                static void updateBaseline(const std::string &testName,
                                           double newExpectedTime,
                                           size_t newExpectedMemory);

                // 生成回归测试报告
                static void generateReport(const std::vector<RegressionResult> &results);

            private:
                static std::vector<Baseline> loadBaselines();
                static void saveBaselines(const std::vector<Baseline> &baselines);

                // 执行单个回归测试
                static RegressionResult runSingleTest(const Baseline &baseline);

                // 检查性能退化
                static bool checkPerformanceRegression(const RegressionResult &result);

                // 生成测试数据
                static std::string generateTestInput(const std::string &testName);
            };

        } // namespace regression
    } // namespace frontend
} // namespace rp
