#pragma once
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

// 前向声明全局操作符
void* operator new(size_t size, const char* file, int line, const char* function);
void operator delete(void* ptr) noexcept;

namespace rp {
    namespace frontend {
        namespace memory {

            // 内存分配跟踪器
            class MemoryTracker {
              public:
                // 内存分配信息
                struct AllocationInfo {
                    void* address;
                    size_t size;
                    std::string file;
                    int line;
                    std::string function;
                    bool isDeleted;
                    std::string stackTrace;
                };

                // 内存泄漏报告
                struct LeakReport {
                    size_t totalLeaks;
                    size_t totalLeakedBytes;
                    std::vector<AllocationInfo> leaks;
                };

                // 初始化内存跟踪器
                static void initialize();

                // 记录内存分配
                static void recordAllocation(void* ptr, size_t size, const char* file, int line, const char* function);

                // 记录内存释放
                static void recordDeallocation(void* ptr);

                // 生成内存泄漏报告
                static LeakReport generateLeakReport();

                // 检查特定地址是否有效
                static bool isValidPointer(void* ptr);

                // 获取当前内存使用统计
                static size_t getCurrentUsage();
                static size_t getPeakUsage();

                // 重置跟踪器
                static void reset();

              public:
                // 内存边界常量
                static const size_t BOUNDARY_SIZE = 8;
                static const unsigned char BOUNDARY_PATTERN[8];

              private:
                static std::unordered_map<void*, AllocationInfo> allocations;
                static size_t currentUsage;
                static size_t peakUsage;

                // 获取调用栈
                static std::string captureStackTrace();

                // 检查内存边界
                static bool checkMemoryBoundaries(void* ptr);
            };

        }  // namespace memory
    }  // namespace frontend
}  // namespace rp

// 宏定义用于替换 new 操作符
#ifdef DEBUG_MEMORY
#define new new (__FILE__, __LINE__, __FUNCTION__)
#endif
