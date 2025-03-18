#pragma once
#include "LexerMemoryTracker.h"

namespace rp {
    namespace frontend {
        namespace memory {
            namespace reporting {
                // 内存报告相关功能
                void updateUsageStats(size_t size, bool isAllocation);
                void sortLeaksBySize(std::vector<MemoryTracker::AllocationInfo>& leaks);
                void printAllocationInfo(const MemoryTracker::AllocationInfo& info);
            }  // namespace reporting
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
