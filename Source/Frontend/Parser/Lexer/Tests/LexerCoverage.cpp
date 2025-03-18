#include "LexerCoverage.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace rp {
    namespace frontend {
        namespace coverage {

            // 静态成员初始化
            std::unordered_map<std::string, LexerCoverage::FileCoverage> LexerCoverage::coverage;
            std::set<std::string> LexerCoverage::instrumentedFiles;

            void LexerCoverage::initialize() {
                coverage.clear();
                instrumentedFiles.clear();
            }

            void LexerCoverage::recordExecution(const std::string &file, int line) {
                std::filesystem::path filePath = std::filesystem::absolute(file);
                if (coverage.find(filePath.string()) == coverage.end()) {
                    parseSourceFile(filePath.string());
                }

                auto &fileCov = coverage[filePath.string()];
                if (fileCov.lines.find(line) != fileCov.lines.end()) {
                    fileCov.lines[line].executionCount++;
                }
            }

            void LexerCoverage::recordFunctionEntry(const std::string &function) {
                for (auto &[file, fileCov] : coverage) {
                    for (auto &func : fileCov.functions) {
                        if (func.name == function) {
                            func.executionCount++;
                            break;
                        }
                    }
                }
            }

            void LexerCoverage::recordBranchExecution(const std::string &file, int line, int branch) {
                std::filesystem::path filePath = std::filesystem::absolute(file);
                if (coverage.find(filePath.string()) == coverage.end()) {
                    parseSourceFile(filePath.string());
                }

                auto &fileCov = coverage[filePath.string()];
                auto &lineInfo = fileCov.lines[line];
                if (lineInfo.isBranchPoint) {
                    if (std::find(lineInfo.branchTargets.begin(), lineInfo.branchTargets.end(), branch) ==
                        lineInfo.branchTargets.end()) {
                        lineInfo.branchTargets.push_back(branch);
                    }
                }
            }

            void LexerCoverage::parseSourceFile(const std::string &filename) {
                if (instrumentedFiles.find(filename) != instrumentedFiles.end()) {
                    return;
                }

                std::ifstream file(filename);
                if (!file.is_open()) {
                    std::cerr << "Failed to open source file: " << filename << std::endl;
                    return;
                }

                FileCoverage fileCov;
                fileCov.filename = filename;

                std::string line;
                int lineNumber = 0;
                FunctionInfo *currentFunction = nullptr;
                int braceCount = 0;

                while (std::getline(file, line)) {
                    lineNumber++;
                    LineInfo lineInfo;
                    lineInfo.lineNumber = lineNumber;
                    lineInfo.executionCount = 0;
                    lineInfo.sourceCode = line;
                    lineInfo.isBranchPoint = false;

                    // 函数检测
                    if (isFunctionDeclaration(line)) {
                        auto funcInfo = parseFunctionDeclaration(line, lineNumber);
                        fileCov.functions.push_back(funcInfo);
                        currentFunction = &fileCov.functions.back();
                        braceCount = 0;
                    }

                    // 分支点检测
                    if (isBranchPoint(line)) {
                        lineInfo.isBranchPoint = true;
                    }

                    // 更新大括号计数
                    braceCount += countBraces(line);

                    fileCov.lines[lineNumber] = lineInfo;
                    if (currentFunction) {
                        currentFunction->lines.push_back(lineInfo);
                    }

                    // 检测函数结束
                    if (currentFunction && braceCount <= 0) {
                        currentFunction->endLine = lineNumber;
                        currentFunction = nullptr;
                    }
                }

                coverage[filename] = fileCov;
                instrumentedFiles.insert(filename);
            }

            void LexerCoverage::generateReport(const std::string &outputFile) { generateHtmlReport(outputFile); }

            void LexerCoverage::generateHtmlReport(const std::string &outputFile) {
                std::ofstream out(outputFile);
                if (!out.is_open()) {
                    std::cerr << "Failed to create report file: " << outputFile << std::endl;
                    return;
                }

                writeHtmlHeader(out);
                writeOverallStatistics(out);
                writeFileDetails(out);
                writeHtmlFooter(out);
            }

            void LexerCoverage::writeHtmlHeader(std::ofstream &out) {
                out << "<!DOCTYPE html>\n"
                    << "<html>\n<head>\n"
                    << "<title>Lexer Coverage Report</title>\n"
                    << "<style>\n"
                    << "body { font-family: Arial, sans-serif; margin: 20px; }\n"
                    << ".covered { background-color: #90EE90; }\n"
                    << ".uncovered { background-color: #FFB6C1; }\n"
                    << ".branch { background-color: #ADD8E6; }\n"
                    << ".function { font-weight: bold; margin-top: 20px; }\n"
                    << ".stats { margin: 20px 0; padding: 10px; background-color: #f0f0f0; }\n"
                    << "</style>\n</head>\n<body>\n";
            }

            void LexerCoverage::writeOverallStatistics(std::ofstream &out) {
                auto stats = calculateStats();
                out << "<h1>Lexer Coverage Report</h1>\n"
                    << "<div class='stats'>\n"
                    << "<h2>Overall Statistics</h2>\n"
                    << "Line Coverage: " << stats.getLineCoverage() << "%<br>\n"
                    << "Branch Coverage: " << stats.getBranchCoverage() << "%<br>\n"
                    << "Function Coverage: " << stats.getFunctionCoverage() << "%<br>\n"
                    << "</div>\n";
            }

            void LexerCoverage::writeFileDetails(std::ofstream &out) {
                for (const auto &[filename, fileCov] : coverage) {
                    out << "<h2>File: " << std::filesystem::path(filename).filename().string() << "</h2>\n"
                        << "<div class='stats'>\n"
                        << "Line Coverage: " << fileCov.stats.getLineCoverage() << "%<br>\n"
                        << "Branch Coverage: " << fileCov.stats.getBranchCoverage() << "%<br>\n"
                        << "Function Coverage: " << fileCov.stats.getFunctionCoverage() << "%<br>\n"
                        << "</div>\n"
                        << "<pre>\n";

                    writeSourceCode(out, fileCov);
                    out << "</pre>\n";
                }
            }

            void LexerCoverage::writeSourceCode(std::ofstream &out, const FileCoverage &fileCov) {
                for (const auto &[lineNum, lineInfo] : fileCov.lines) {
                    std::string cssClass = lineInfo.executionCount > 0 ? "covered" : "uncovered";
                    if (lineInfo.isBranchPoint) {
                        cssClass += " branch";
                    }

                    out << "<div class='" << cssClass << "'>" << lineNum << ": "
                        << (lineInfo.executionCount > 0 ? lineInfo.executionCount : 0) << " | " << lineInfo.sourceCode
                        << "</div>\n";
                }
            }

            void LexerCoverage::writeHtmlFooter(std::ofstream &out) { out << "</body>\n</html>\n"; }

            LexerCoverage::CoverageStats LexerCoverage::calculateStats() {
                CoverageStats stats{0, 0, 0, 0, 0, 0};

                for (const auto &[filename, fileCov] : coverage) {
                    // 行统计
                    for (const auto &[lineNum, lineInfo] : fileCov.lines) {
                        stats.totalLines++;
                        if (lineInfo.executionCount > 0) {
                            stats.coveredLines++;
                        }
                        if (lineInfo.isBranchPoint) {
                            stats.totalBranches++;
                            if (!lineInfo.branchTargets.empty()) {
                                stats.coveredBranches++;
                            }
                        }
                    }

                    // 函数统计
                    for (const auto &func : fileCov.functions) {
                        stats.totalFunctions++;
                        if (func.executionCount > 0) {
                            stats.coveredFunctions++;
                        }
                    }
                }

                return stats;
            }

            void LexerCoverage::reset() {
                coverage.clear();
                instrumentedFiles.clear();
            }

            bool LexerCoverage::isFunctionDeclaration(const std::string &line) {
                static const std::vector<std::string> functionKeywords = {"void",
                                                                          "int",
                                                                          "bool",
                                                                          "char",
                                                                          "float",
                                                                          "double",
                                                                          "auto",
                                                                          "static",
                                                                          "inline",
                                                                          "virtual",
                                                                          "constexpr"};

                for (const auto &keyword : functionKeywords) {
                    if (line.find(keyword) != std::string::npos && line.find('(') != std::string::npos &&
                        line.find(';') == std::string::npos) {
                        return true;
                    }
                }
                return false;
            }

            LexerCoverage::FunctionInfo LexerCoverage::parseFunctionDeclaration(const std::string &line,
                                                                                int lineNumber) {
                FunctionInfo func;
                func.startLine = lineNumber;
                func.executionCount = 0;

                size_t nameStart = line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
                size_t nameEnd = line.find_first_of("(", nameStart);
                if (nameStart != std::string::npos && nameEnd != std::string::npos) {
                    func.name = line.substr(nameStart, nameEnd - nameStart);
                }

                return func;
            }

            int LexerCoverage::countBraces(const std::string &line) {
                int count = 0;
                for (char c : line) {
                    if (c == '{')
                        count++;
                    else if (c == '}')
                        count--;
                }
                return count;
            }

            bool LexerCoverage::isBranchPoint(const std::string &line) {
                // 去除前导空格
                size_t start = line.find_first_not_of(" \t");
                if (start == std::string::npos) {
                    return false;
                }

                std::string trimmed = line.substr(start);

                // 检查常见的分支关键字
                static const std::vector<std::string> branchKeywords = {
                    "if", "else", "for", "while", "do", "switch", "case", "default"};

                for (const auto &keyword : branchKeywords) {
                    // 确保关键字后面跟着空格或括号，避免匹配变量名中的关键字
                    size_t pos = trimmed.find(keyword);
                    if (pos == 0) {                                  // 关键字在行首
                        if (trimmed.length() == keyword.length() ||  // 整行就是关键字
                            trimmed[keyword.length()] == ' ' ||      // 关键字后跟空格
                            trimmed[keyword.length()] == '(' ||      // 关键字后跟左括号
                            trimmed[keyword.length()] == '{' ||      // 关键字后跟左大括号
                            trimmed[keyword.length()] == ':') {      // case和default后跟冒号
                            return true;
                        }
                    }
                }

                // 检查条件运算符 ?:
                if (trimmed.find('?') != std::string::npos && trimmed.find(':') != std::string::npos) {
                    return true;
                }

                return false;
            }

        }  // namespace coverage
    }  // namespace frontend
}  // namespace rp
