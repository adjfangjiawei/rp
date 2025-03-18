#include "Diagnostic.h"
#include <iostream>

namespace rp
{
    namespace frontend
    {

        std::string DiagnosticMessage::format() const
        {
            std::stringstream ss;

            // 格式化级别
            switch (level)
            {
            case DiagnosticLevel::Note:
                ss << "note: ";
                break;
            case DiagnosticLevel::Warning:
                ss << "warning: ";
                break;
            case DiagnosticLevel::Error:
                ss << "error: ";
                break;
            case DiagnosticLevel::Fatal:
                ss << "fatal error: ";
                break;
            }

            // 添加位置信息和消息
            ss << location.toString() << ": " << message << "\n";

            // 添加修复建议
            if (!fixes.empty())
            {
                ss << "suggested fixes:\n";
                for (const auto &fix : fixes)
                {
                    ss << "  - " << fix << "\n";
                }
            }

            return ss.str();
        }

        DiagnosticEngine::DiagnosticEngine()
            : errorCount(0), warningCount(0) {}

        void DiagnosticEngine::report(DiagnosticLevel level,
                                      const SourceLocation &loc,
                                      const std::string &message)
        {
            auto diag = std::make_shared<DiagnosticMessage>(level, loc, message);
            diagnostics.push_back(diag);
            currentDiagnostic = diag;

            // 更新计数
            if (level == DiagnosticLevel::Error || level == DiagnosticLevel::Fatal)
            {
                ++errorCount;
            }
            else if (level == DiagnosticLevel::Warning)
            {
                ++warningCount;
            }

            // 输出诊断信息
            std::cerr << diag->format();
        }

        void DiagnosticEngine::addFix(const std::string &fix)
        {
            if (currentDiagnostic)
            {
                currentDiagnostic->addFix(fix);
            }
        }

        void DiagnosticEngine::clear()
        {
            diagnostics.clear();
            errorCount = 0;
            warningCount = 0;
            currentDiagnostic.reset();
        }

    } // namespace frontend
} // namespace rp
