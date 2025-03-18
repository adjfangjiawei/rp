#pragma once
#include <mutex>

#include "LexerMemoryTracker.h"

namespace rp {
    namespace frontend {
        namespace memory {
            // 核心内存跟踪功能的实现声明
            extern std::unordered_map<void*, MemoryTracker::AllocationInfo> allocations;
            extern size_t currentUsage;
            extern size_t peakUsage;
            extern std::mutex trackerMutex;
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
