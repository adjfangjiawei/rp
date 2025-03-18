#pragma once
#include <random>
#include <string>

#ifdef _WIN32
#include <psapi.h>
#include <windows.h>
#else
#include <sys/resource.h>
#include <sys/time.h>
#endif

namespace rp {
    namespace frontend {
        namespace test {

            class LexerTestUtils {
              public:
                // 生成随机代码
                static std::string generateRandomCode(size_t length) {
                    static const char* tokens[] = {
                        "if",     "else", "while", "for", "return", "class", "struct", "int",        "float",
                        "double", "char", "void",  "+",   "-",      "*",     "/",      "=",          "==",
                        "!=",     "<",    ">",     "(",   ")",      "{",     "}",      "[",          "]",
                        ";",      ",",    ".",     "0",   "1",      "42",    "3.14",   "\"string\"", "'c'"};
                    static const size_t numTokens = sizeof(tokens) / sizeof(tokens[0]);

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, numTokens - 1);

                    std::string code;
                    size_t currentLength = 0;

                    while (currentLength < length) {
                        std::string token = tokens[dis(gen)];
                        code += token + " ";
                        currentLength += token.length() + 1;
                    }

                    return code;
                }

                // 内存使用统计结构
                struct MemoryStats {
                    size_t peakMemoryUsage;
                    size_t currentMemoryUsage;
                    size_t totalAllocations;
                    size_t totalDeallocations;
                };

                // 获取当前内存使用情况
                static MemoryStats getMemoryStats() {
                    MemoryStats stats{0, 0, 0, 0};
#ifdef _WIN32
                    // Windows实现
                    PROCESS_MEMORY_COUNTERS_EX pmc;
                    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
                        stats.currentMemoryUsage = pmc.WorkingSetSize;
                        stats.peakMemoryUsage = pmc.PeakWorkingSetSize;
                    }
#else
                    // Linux实现
                    struct rusage usage;
                    if (getrusage(RUSAGE_SELF, &usage) == 0) {
                        stats.currentMemoryUsage = usage.ru_maxrss * 1024;
                        stats.peakMemoryUsage = stats.currentMemoryUsage;
                    }
#endif
                    return stats;
                }

                // 生成特定长度的标识符
                static std::string generateIdentifier(size_t length) {
                    std::string identifier;
                    identifier.reserve(length);
                    identifier += 'a';  // 确保标识符以字母开头
                    for (size_t i = 1; i < length; ++i) {
                        identifier += 'a' + (i % 26);
                    }
                    return identifier;
                }

                // 生成嵌套注释
                static std::string generateNestedComments(int depth) {
                    std::string result = "/* level 0";
                    for (int i = 1; i <= depth; ++i) {
                        result += " /* level " + std::to_string(i);
                    }
                    for (int i = depth; i >= 0; --i) {
                        result += " */ ";
                    }
                    return result;
                }
            };

        }  // namespace test
    }  // namespace frontend
}  // namespace rp
