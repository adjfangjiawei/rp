#include "Diagnostic.h"

#include <iostream>

namespace rp {
    namespace frontend {

        std::string DiagnosticMessage::format() const {
            std::stringstream ss;

            // 格式化级别和位置信息
            ss << location.filename << ":" << location.line << ":" << location.column << ": ";

            switch (level) {
                case DiagnosticLevel::Note:
                    ss << "\033[1;36mnote:\033[0m ";  // 青色
                    break;
                case DiagnosticLevel::Warning:
                    ss << "\033[1;33mwarning:\033[0m ";  // 黄色
                    break;
                case DiagnosticLevel::Error:
                    ss << "\033[1;31merror:\033[0m ";  // 红色
                    break;
                case DiagnosticLevel::Fatal:
                    ss << "\033[1;35mfatal error:\033[0m ";  // 紫色
                    break;
            }

            // 添加消息
            ss << message << "\n";

            // 如果消息中包含特定的错误类型标识，添加相应的帮助信息
            if (message.find("无效的转义序列") != std::string::npos) {
                ss << "\033[1;32m提示：\033[0m\n";
                ss << "  标准转义序列包括：\n";
                ss << "    \\n  - 换行\n";
                ss << "    \\t  - 制表符\n";
                ss << "    \\r  - 回车\n";
                ss << "    \\b  - 退格\n";
                ss << "    \\f  - 换页\n";
                ss << "    \\v  - 垂直制表符\n";
                ss << "    \\a  - 响铃\n";
                ss << "    \\\\  - 反斜杠\n";
                ss << "    \\'  - 单引号\n";
                ss << "    \\\"  - 双引号\n";
                ss << "    \\?  - 问号\n";
                ss << "    \\xHH     - 十六进制转义 (HH 是两位十六进制数)\n";
                ss << "    \\uHHHH   - Unicode转义 (HHHH 是四位十六进制数)\n";
                ss << "    \\UHHHHHHHH - Unicode转义 (HHHHHHHH 是八位十六进制数)\n";
            }

            // 添加修复建议
            if (!fixes.empty()) {
                ss << "suggested fixes:\n";
                for (const auto &fix : fixes) {
                    ss << "  - " << fix << "\n";
                }
            }

            return ss.str();
        }

        DiagnosticEngine::DiagnosticEngine() : errorCount(0), warningCount(0) {}

        void DiagnosticEngine::report(DiagnosticLevel level, const SourceLocation &loc, const std::string &message) {
            auto diag = std::make_shared<DiagnosticMessage>(level, loc, message);
            diagnostics.push_back(diag);
            currentDiagnostic = diag;

            // 更新计数
            if (level == DiagnosticLevel::Error || level == DiagnosticLevel::Fatal) {
                ++errorCount;
            } else if (level == DiagnosticLevel::Warning) {
                ++warningCount;
            }

            // 输出诊断信息
            std::cerr << diag->format();
        }

        void DiagnosticEngine::addFix(const std::string &fix) {
            if (currentDiagnostic) {
                currentDiagnostic->addFix(fix);
            }
        }

        void DiagnosticEngine::clear() {
            diagnostics.clear();
            errorCount = 0;
            warningCount = 0;
            currentDiagnostic.reset();
        }

    }  // namespace frontend
}  // namespace rp
