#include "LexerMemoryTracker.h"

#include <cxxabi.h>
#include <execinfo.h>

#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace rp {
    namespace frontend {
        namespace memory {

            // 静态成员初始化
            std::unordered_map<void *, MemoryTracker::AllocationInfo> MemoryTracker::allocations;
            size_t MemoryTracker::currentUsage = 0;
            size_t MemoryTracker::peakUsage = 0;

            // 初始化边界模式
            const unsigned char MemoryTracker::BOUNDARY_PATTERN[BOUNDARY_SIZE] = {
                0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE};

            // 互斥锁，用于线程安全
            static std::mutex trackerMutex;

            void MemoryTracker::initialize() {
                std::lock_guard<std::mutex> lock(trackerMutex);
                allocations.clear();
                currentUsage = 0;
                peakUsage = 0;
            }

            void MemoryTracker::recordAllocation(
                void *ptr, size_t size, const char *file, int line, const char *function) {
                std::lock_guard<std::mutex> lock(trackerMutex);

                // 创建分配信息
                AllocationInfo info;
                info.address = ptr;
                info.size = size;
                info.file = file;
                info.line = line;
                info.function = function;
                info.isDeleted = false;
                info.stackTrace = captureStackTrace();

                // 更新内存使用统计
                currentUsage += size;
                if (currentUsage > peakUsage) {
                    peakUsage = currentUsage;
                }

                // 添加内存边界标记
                unsigned char *boundary = static_cast<unsigned char *>(ptr);
                std::memcpy(boundary - BOUNDARY_SIZE, BOUNDARY_PATTERN, BOUNDARY_SIZE);
                std::memcpy(boundary + size, BOUNDARY_PATTERN, BOUNDARY_SIZE);

                allocations[ptr] = info;
            }

            void MemoryTracker::recordDeallocation(void *ptr) {
                std::lock_guard<std::mutex> lock(trackerMutex);

                auto it = allocations.find(ptr);
                if (it != allocations.end()) {
                    // 检查内存边界
                    if (!checkMemoryBoundaries(ptr)) {
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

            MemoryTracker::LeakReport MemoryTracker::generateLeakReport() {
                std::lock_guard<std::mutex> lock(trackerMutex);

                LeakReport report;
                report.totalLeaks = 0;
                report.totalLeakedBytes = 0;

                for (const auto &[ptr, info] : allocations) {
                    if (!info.isDeleted) {
                        report.totalLeaks++;
                        report.totalLeakedBytes += info.size;
                        report.leaks.push_back(info);
                    }
                }

                // 按大小排序泄漏信息
                std::sort(report.leaks.begin(),
                          report.leaks.end(),
                          [](const AllocationInfo &a, const AllocationInfo &b) { return a.size > b.size; });

                return report;
            }

            bool MemoryTracker::isValidPointer(void *ptr) {
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

            void MemoryTracker::reset() {
                std::lock_guard<std::mutex> lock(trackerMutex);

                // 检查是否有未释放的内存
                LeakReport report = generateLeakReport();
                if (report.totalLeaks > 0) {
                    std::cerr << "Memory leaks detected during reset!\n";
                    std::cerr << "Total leaks: " << report.totalLeaks << "\n";
                    std::cerr << "Total leaked bytes: " << report.totalLeakedBytes << "\n";
                }

                allocations.clear();
                currentUsage = 0;
                peakUsage = 0;
            }

            std::string MemoryTracker::captureStackTrace() {
                const int MAX_FRAMES = 32;
                void *frames[MAX_FRAMES];
                int numFrames = backtrace(frames, MAX_FRAMES);
                char **symbols = backtrace_symbols(frames, numFrames);

                std::stringstream ss;
                for (int i = 1; i < numFrames; ++i) {  // Skip first frame (this function)
                    std::string symbol(symbols[i]);

                    // 尝试解析C++符号名
                    size_t nameStart = symbol.find('(');
                    size_t nameEnd = symbol.find('+', nameStart);
                    if (nameStart != std::string::npos && nameEnd != std::string::npos) {
                        std::string mangledName = symbol.substr(nameStart + 1, nameEnd - nameStart - 1);
                        int status;
                        char *demangledName = abi::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);
                        if (status == 0 && demangledName) {
                            ss << "    " << i << ": " << demangledName << "\n";
                            free(demangledName);
                        } else {
                            ss << "    " << i << ": " << symbol << "\n";
                        }
                    } else {
                        ss << "    " << i << ": " << symbol << "\n";
                    }
                }

                free(symbols);
                return ss.str();
            }

            bool MemoryTracker::checkMemoryBoundaries(void *ptr) {
                auto it = allocations.find(ptr);
                if (it == allocations.end()) {
                    return false;
                }

                unsigned char *boundary = static_cast<unsigned char *>(ptr);

                // 检查前边界
                if (std::memcmp(boundary - BOUNDARY_SIZE, BOUNDARY_PATTERN, BOUNDARY_SIZE) != 0) {
                    return false;
                }

                // 检查后边界
                if (std::memcmp(boundary + it->second.size, BOUNDARY_PATTERN, BOUNDARY_SIZE) != 0) {
                    return false;
                }

                return true;
            }

        }  // namespace memory
    }  // namespace frontend
}  // namespace rp

// 全局operator new重载
void *operator new(size_t size, const char *file, int line, const char *function) {
    // 分配额外的空间用于边界检查
    void *ptr = std::malloc(size + 2 * rp::frontend::memory::MemoryTracker::BOUNDARY_SIZE);
    if (!ptr) {
        throw std::bad_alloc();
    }

    // 调整指针以留出前边界空间
    void *userPtr = static_cast<unsigned char *>(ptr) + rp::frontend::memory::MemoryTracker::BOUNDARY_SIZE;

    // 记录分配
    rp::frontend::memory::MemoryTracker::recordAllocation(userPtr, size, file, line, function);

    return userPtr;
}

// 全局operator delete重载
void operator delete(void *ptr) noexcept {
    if (!ptr) return;

    // 记录释放
    rp::frontend::memory::MemoryTracker::recordDeallocation(ptr);

    // 调整指针以释放实际分配的内存
    void *actualPtr = static_cast<unsigned char *>(ptr) - rp::frontend::memory::MemoryTracker::BOUNDARY_SIZE;
    std::free(actualPtr);
}
