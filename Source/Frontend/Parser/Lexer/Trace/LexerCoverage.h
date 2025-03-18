#ifndef RP_FRONTEND_LEXER_COVERAGE_H
#define RP_FRONTEND_LEXER_COVERAGE_H

#include <string>
#include <unordered_map>
#include <vector>

namespace rp {
    namespace frontend {
        namespace coverage {

            class LexerCoverage {
              public:
                // 初始化覆盖率追踪系统
                static void initialize();

                // 记录执行的行
                static void recordExecution(const std::string& file, int line);

                // 生成覆盖率报告
                static void generateReport(const std::string& outputFile);

              private:
                static std::unordered_map<std::string, std::vector<bool>> fileLineCoverage;
                static std::unordered_map<std::string, std::vector<int>> executionCount;
            };

        }  // namespace coverage
    }  // namespace frontend
}  // namespace rp

#endif  // RP_FRONTEND_LEXER_COVERAGE_H
