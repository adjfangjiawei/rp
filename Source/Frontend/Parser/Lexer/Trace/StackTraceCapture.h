#pragma once
#include <string>

namespace rp {
    namespace frontend {
        namespace memory {
            namespace stacktrace {
                // 调用栈捕获相关功能
                std::string captureStackTrace();
                std::string demangleSymbol(const std::string& symbol);
            }  // namespace stacktrace
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
