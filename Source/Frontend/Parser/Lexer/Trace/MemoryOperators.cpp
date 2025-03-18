#include <cstdlib>
#include <new>

#include "LexerMemoryTracker.h"

// 全局operator new重载
void* operator new(size_t size, const char* file, int line, const char* function) {
    // 分配额外的空间用于边界检查
    void* ptr = std::malloc(size + 2 * rp::frontend::memory::MemoryTracker::BOUNDARY_SIZE);
    if (!ptr) {
        throw std::bad_alloc();
    }

    // 调整指针以留出前边界空间
    void* userPtr = static_cast<unsigned char*>(ptr) + rp::frontend::memory::MemoryTracker::BOUNDARY_SIZE;

    // 记录分配
    rp::frontend::memory::MemoryTracker::recordAllocation(userPtr, size, file, line, function);

    return userPtr;
}

// 全局operator delete重载
void operator delete(void* ptr) noexcept {
    if (!ptr) return;

    // 记录释放
    rp::frontend::memory::MemoryTracker::recordDeallocation(ptr);

    // 调整指针以释放实际分配的内存
    void* actualPtr = static_cast<unsigned char*>(ptr) - rp::frontend::memory::MemoryTracker::BOUNDARY_SIZE;
    std::free(actualPtr);
}
