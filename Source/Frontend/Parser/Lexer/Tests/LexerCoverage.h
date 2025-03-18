#pragma once
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Frontend/Parser/Lexer/MainLexer/Lexer.h"

namespace rp {
    namespace frontend {
        namespace coverage {

            /**
             * @brief 代码覆盖率分析器
             *
             * 用于跟踪和分析Lexer代码的执行覆盖情况，包括：
             * - 行覆盖率
             * - 分支覆盖率
             * - 函数覆盖率
             */
            class LexerCoverage {
              public:
                /**
                 * @brief 覆盖率统计信息
                 */
                struct CoverageStats {
                    size_t totalLines{0};        ///< 总行数
                    size_t coveredLines{0};      ///< 已覆盖行数
                    size_t totalBranches{0};     ///< 总分支数
                    size_t coveredBranches{0};   ///< 已覆盖分支数
                    size_t totalFunctions{0};    ///< 总函数数
                    size_t coveredFunctions{0};  ///< 已覆盖函数数

                    // 计算覆盖率百分比
                    double getLineCoverage() const {
                        return totalLines > 0 ? (coveredLines * 100.0 / totalLines) : 0.0;
                    }
                    double getBranchCoverage() const {
                        return totalBranches > 0 ? (coveredBranches * 100.0 / totalBranches) : 0.0;
                    }
                    double getFunctionCoverage() const {
                        return totalFunctions > 0 ? (coveredFunctions * 100.0 / totalFunctions) : 0.0;
                    }
                };

                /**
                 * @brief 代码行覆盖信息
                 */
                struct LineInfo {
                    int lineNumber{0};               ///< 行号
                    int executionCount{0};           ///< 执行次数
                    std::string sourceCode;          ///< 源代码
                    bool isBranchPoint{false};       ///< 是否是分支点
                    std::vector<int> branchTargets;  ///< 分支目标
                };

                /**
                 * @brief 函数覆盖信息
                 */
                struct FunctionInfo {
                    std::string name;             ///< 函数名
                    int startLine{0};             ///< 起始行
                    int endLine{0};               ///< 结束行
                    int executionCount{0};        ///< 执行次数
                    std::vector<LineInfo> lines;  ///< 函数包含的行
                };

                /**
                 * @brief 文件覆盖信息
                 */
                struct FileCoverage {
                    std::string filename;                     ///< 文件名
                    std::vector<FunctionInfo> functions;      ///< 函数列表
                    std::unordered_map<int, LineInfo> lines;  ///< 行信息
                    CoverageStats stats;                      ///< 统计信息
                };

                // 基本操作
                static void initialize();  ///< 初始化覆盖率分析器
                static void reset();       ///< 重置覆盖率数据

                // 记录执行信息
                static void recordExecution(const std::string& file, int line);
                static void recordFunctionEntry(const std::string& function);
                static void recordBranchExecution(const std::string& file, int line, int branch);

                // 报告生成
                static void generateReport(const std::string& outputFile);
                static CoverageStats calculateStats();

              private:
                // 静态成员
                static std::unordered_map<std::string, FileCoverage> coverage;
                static std::set<std::string> instrumentedFiles;

                // 文件解析
                static void parseSourceFile(const std::string& filename);
                static bool isFunctionDeclaration(const std::string& line);
                static FunctionInfo parseFunctionDeclaration(const std::string& line, int lineNumber);
                static bool isBranchPoint(const std::string& line);
                static int countBraces(const std::string& line);

                // HTML报告生成
                static void generateHtmlReport(const std::string& outputFile);
                static void writeHtmlHeader(std::ofstream& out);
                static void writeOverallStatistics(std::ofstream& out);
                static void writeFileDetails(std::ofstream& out);
                static void writeSourceCode(std::ofstream& out, const FileCoverage& fileCov);
                static void writeHtmlFooter(std::ofstream& out);
            };

        }  // namespace coverage
    }  // namespace frontend
}  // namespace rp
