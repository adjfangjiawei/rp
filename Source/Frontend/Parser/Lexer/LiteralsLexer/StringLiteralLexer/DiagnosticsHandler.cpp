#include "DiagnosticsHandler.h"

#include "Frontend/Diagnostic/Diagnostic.h"

namespace rp {
    namespace frontend {

        DiagnosticsHandler::DiagnosticsHandler(const std::shared_ptr<DiagnosticEngine>& diagnosticsEngine)
            : diagnostics(diagnosticsEngine) {}

        void DiagnosticsHandler::handleDiagnostics(const std::string& error,
                                                   const std::vector<std::string>& warnings,
                                                   const SourceLocation& loc) {
            // 处理错误信息
            if (!error.empty()) {
                reportError(error, loc);
            }

            // 处理所有警告信息
            for (const auto& warning : warnings) {
                reportWarning(warning, loc);
            }
        }

        void DiagnosticsHandler::reportError(const std::string& error, const SourceLocation& loc) {
            if (diagnostics) {
                diagnostics->report(DiagnosticLevel::Error, loc, error);
            }
        }

        void DiagnosticsHandler::reportWarning(const std::string& warning, const SourceLocation& loc) {
            if (diagnostics) {
                diagnostics->report(DiagnosticLevel::Warning, loc, warning);
            }
        }

    }  // namespace frontend
}  // namespace rp
