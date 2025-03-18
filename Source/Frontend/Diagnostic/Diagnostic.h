#pragma once
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace rp
{
    namespace frontend
    {

        // 诊断级别
        enum class DiagnosticLevel
        {
            Note,    // 提示信息
            Warning, // 警告
            Error,   // 错误
            Fatal    // 致命错误
        };

        // 诊断位置信息
        struct SourceLocation
        {
            std::string filename;
            unsigned line;
            unsigned column;

            std::string toString() const
            {
                std::stringstream ss;
                ss << filename << ":" << line << ":" << column;
                return ss.str();
            }
        };

        // 诊断信息
        class DiagnosticMessage
        {
        public:
            DiagnosticMessage(DiagnosticLevel level,
                              const SourceLocation &loc,
                              const std::string &msg)
                : level(level), location(loc), message(msg) {}

            DiagnosticLevel getLevel() const { return level; }
            const SourceLocation &getLocation() const { return location; }
            const std::string &getMessage() const { return message; }

            // 获取修复建议
            const std::vector<std::string> &getFixes() const { return fixes; }
            void addFix(const std::string &fix) { fixes.push_back(fix); }

            // 格式化诊断信息
            std::string format() const;

        private:
            DiagnosticLevel level;
            SourceLocation location;
            std::string message;
            std::vector<std::string> fixes; // 可能的修复建议
        };

        // 诊断引擎
        class DiagnosticEngine
        {
        public:
            DiagnosticEngine();

            // 报告诊断信息
            void report(DiagnosticLevel level,
                        const SourceLocation &loc,
                        const std::string &message);

            // 添加修复建议
            void addFix(const std::string &fix);

            // 获取诊断信息
            const std::vector<std::shared_ptr<DiagnosticMessage>> &getDiagnostics() const
            {
                return diagnostics;
            }

            // 检查是否有错误
            bool hasErrors() const { return errorCount > 0; }

            // 获取错误计数
            unsigned getErrorCount() const { return errorCount; }
            unsigned getWarningCount() const { return warningCount; }

            // 清除所有诊断信息
            void clear();

        private:
            std::vector<std::shared_ptr<DiagnosticMessage>> diagnostics;
            unsigned errorCount;
            unsigned warningCount;
            std::shared_ptr<DiagnosticMessage> currentDiagnostic;
        };

    } // namespace frontend
} // namespace rp
