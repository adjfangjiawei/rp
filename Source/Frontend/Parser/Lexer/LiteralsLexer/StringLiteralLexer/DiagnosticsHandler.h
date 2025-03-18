
#ifndef RP_DIAGNOSTICS_HANDLER_H
#define RP_DIAGNOSTICS_HANDLER_H

#include <memory>
#include <string>
#include <vector>

#include "Frontend/Diagnostic/Diagnostic.h"

namespace rp {
    namespace frontend {

        class DiagnosticsHandler {
          public:
            // 构造函数，接收诊断引擎
            explicit DiagnosticsHandler(const std::shared_ptr<DiagnosticEngine>& diagnostics);

            // 处理诊断信息
            void handleDiagnostics(const std::string& error,
                                   const std::vector<std::string>& warnings,
                                   const SourceLocation& loc);

            // 报告错误
            void reportError(const std::string& error, const SourceLocation& loc);

            // 报告警告
            void reportWarning(const std::string& warning, const SourceLocation& loc);

          private:
            std::shared_ptr<DiagnosticEngine> diagnostics;
        };

    }  // namespace frontend
}  // namespace rp

#endif  // RP_DIAGNOSTICS_HANDLER_H
