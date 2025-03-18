#pragma once
#include "LexerMemoryTracker.h"

namespace rp {
    namespace frontend {
        namespace memory {
            namespace boundary {
                // 内存边界检查相关功能
                void setupBoundaries(void* ptr, size_t size);
                bool checkBoundaries(void* ptr, size_t size);
            }  // namespace boundary
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
