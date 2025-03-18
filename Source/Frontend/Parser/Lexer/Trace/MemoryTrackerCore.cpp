#include "MemoryTrackerCore.h"

#include <iostream>

#include "MemoryBoundaryChecker.h"
#include "StackTraceCapture.h"

namespace rp {
    namespace frontend {
        namespace memory {
            // 静态成员初始化
            std::unordered_map<void*, MemoryTracker::AllocationInfo> allocations;
            size_t currentUsage = 0;
            size_t peakUsage = 0;
            std::mutex trackerMutex;

            void MemoryTracker::initialize() {
                std::lock_guard<std::mutex> lock(trackerMutex);
                allocations.clear();
                currentUsage = 0;
                peakUsage = 0;
            }

            void MemoryTracker::recordAllocation(
                void* ptr, size_t size, const char* file, int line, const char* function) {
                std::lock_guard<std::mutex> lock(trackerMutex);

                // 创建分配信息
                AllocationInfo info;
                info.address = ptr;
                info.size = size;
                info.file = file;
                info.line = line;
                info.function = function;
                info.isDeleted = false;
                info.stackTrace = stacktrace::captureStackTrace();

                // 更新内存使用统计
                currentUsage += size;
                if (currentUsage > peakUsage) {
                    peakUsage = currentUsage;
                }

                // 设置内存边界
                boundary::setupBoundaries(ptr, size);
                allocations[ptr] = info;
            }

            void MemoryTracker::recordDeallocation(void* ptr) {
                std::lock_guard<std::mutex> lock(trackerMutex);

                auto it = allocations.find(ptr);
                if (it != allocations.end()) {
                    if (!boundary::checkBoundaries(ptr, it->second.size)) {
                        std::cerr << "Memory corruption detected at " << ptr << std::endl;
                        std::cerr << "Allocation info:\n";
                        std::cerr << "File: " << it->second.file << "\n";
                        std::cerr << "Line: " << it->second.line << "\n";
                        std::cerr << "Function: " << it->second.function << "\n";
                        std::cerr << "Stack trace:\n" << it->second.stackTrace << std::endl;
                        std::abort();
                    }

                    currentUsage -= it->second.size;
                    it->second.isDeleted = true;
                    allocations.erase(it);
                } else {
                    std::cerr << "Attempting to free unallocated memory at " << ptr << std::endl;
                    std::abort();
                }
            }

            bool MemoryTracker::isValidPointer(void* ptr) {
                std::lock_guard<std::mutex> lock(trackerMutex);
                return allocations.find(ptr) != allocations.end();
            }

            size_t MemoryTracker::getCurrentUsage() {
                std::lock_guard<std::mutex> lock(trackerMutex);
                return currentUsage;
            }

            size_t MemoryTracker::getPeakUsage() {
                std::lock_guard<std::mutex> lock(trackerMutex);
                return peakUsage;
            }
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
