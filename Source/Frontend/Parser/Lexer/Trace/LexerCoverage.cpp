#include "LexerCoverage.h"

#include <fstream>
#include <iomanip>
#include <sstream>

namespace rp {
    namespace frontend {
        namespace coverage {

            std::unordered_map<std::string, std::vector<bool>> LexerCoverage::fileLineCoverage;
            std::unordered_map<std::string, std::vector<int>> LexerCoverage::executionCount;

            void LexerCoverage::initialize() {
                // 清空之前的覆盖率数据
                fileLineCoverage.clear();
                executionCount.clear();
            }

            void LexerCoverage::recordExecution(const std::string& file, int line) {
                if (line <= 0) return;

                // 确保文件记录存在
                if (fileLineCoverage.find(file) == fileLineCoverage.end()) {
                    fileLineCoverage[file] = std::vector<bool>(10000, false);  // 预分配足够空间
                    executionCount[file] = std::vector<int>(10000, 0);
                }

                // 记录行执行情况
                fileLineCoverage[file][line - 1] = true;
                executionCount[file][line - 1]++;
            }

            void LexerCoverage::generateReport(const std::string& outputFile) {
                std::ofstream out(outputFile);
                if (!out.is_open()) {
                    return;
                }

                // 写入HTML头部
                out << "<!DOCTYPE html>\n"
                    << "<html>\n"
                    << "<head>\n"
                    << "<title>Lexer Coverage Report</title>\n"
                    << "<style>\n"
                    << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                    << ".covered { background-color: #90EE90; }\n"
                    << ".uncovered { background-color: #FFB6C1; }\n"
                    << ".file { margin-bottom: 20px; border: 1px solid #ccc; padding: 10px; }\n"
                    << ".stats { margin-bottom: 10px; }\n"
                    << "pre { margin: 0; white-space: pre-wrap; }\n"
                    << "</style>\n"
                    << "</head>\n"
                    << "<body>\n"
                    << "<h1>Lexer Coverage Report</h1>\n";

                // 生成总体统计
                int totalLines = 0;
                int coveredLines = 0;

                for (const auto& file : fileLineCoverage) {
                    const auto& coverage = file.second;
                    for (size_t i = 0; i < coverage.size(); ++i) {
                        if (executionCount[file.first][i] > 0) {
                            totalLines++;
                            if (coverage[i]) {
                                coveredLines++;
                            }
                        }
                    }
                }

                // 写入总体统计
                double coverage = totalLines > 0 ? (100.0 * coveredLines / totalLines) : 0;
                out << "<div class='stats'>\n"
                    << "<h2>Overall Statistics</h2>\n"
                    << "<p>Total Lines: " << totalLines << "</p>\n"
                    << "<p>Covered Lines: " << coveredLines << "</p>\n"
                    << "<p>Coverage: " << std::fixed << std::setprecision(2) << coverage << "%</p>\n"
                    << "</div>\n";

                // 为每个文件生成详细报告
                for (const auto& file : fileLineCoverage) {
                    const auto& coverage = file.second;
                    const auto& counts = executionCount[file.first];

                    int fileTotal = 0;
                    int fileCovered = 0;

                    for (size_t i = 0; i < coverage.size(); ++i) {
                        if (counts[i] > 0) {
                            fileTotal++;
                            if (coverage[i]) {
                                fileCovered++;
                            }
                        }
                    }

                    double fileCoverage = fileTotal > 0 ? (100.0 * fileCovered / fileTotal) : 0;

                    out << "<div class='file'>\n"
                        << "<h3>" << file.first << "</h3>\n"
                        << "<p>Coverage: " << std::fixed << std::setprecision(2) << fileCoverage << "%</p>\n"
                        << "<p>Lines Covered: " << fileCovered << "/" << fileTotal << "</p>\n"
                        << "<pre>\n";

                    for (size_t i = 0; i < coverage.size(); ++i) {
                        if (counts[i] > 0) {
                            out << "<div class='" << (coverage[i] ? "covered" : "uncovered") << "'>" << std::setw(4)
                                << (i + 1) << ": "
                                << "Executed " << counts[i] << " times"
                                << "</div>\n";
                        }
                    }

                    out << "</pre>\n</div>\n";
                }

                // 写入HTML尾部
                out << "</body>\n</html>";
                out.close();
            }

        }  // namespace coverage
    }  // namespace frontend
}  // namespace rp
