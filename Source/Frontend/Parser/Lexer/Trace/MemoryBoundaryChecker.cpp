#include "MemoryBoundaryChecker.h"

#include <cstring>

#include "MemoryTrackerCore.h"

namespace rp {
    namespace frontend {
        namespace memory {
            namespace boundary {
                void setupBoundaries(void* ptr, size_t size) {
                    unsigned char* boundary = static_cast<unsigned char*>(ptr);
                    std::memcpy(boundary - MemoryTracker::BOUNDARY_SIZE,
                                MemoryTracker::BOUNDARY_PATTERN,
                                MemoryTracker::BOUNDARY_SIZE);
                    std::memcpy(boundary + size, MemoryTracker::BOUNDARY_PATTERN, MemoryTracker::BOUNDARY_SIZE);
                }

                bool checkBoundaries(void* ptr, size_t size) {
                    unsigned char* boundary = static_cast<unsigned char*>(ptr);

                    // 检查前边界
                    if (std::memcmp(boundary - MemoryTracker::BOUNDARY_SIZE,
                                    MemoryTracker::BOUNDARY_PATTERN,
                                    MemoryTracker::BOUNDARY_SIZE) != 0) {
                        return false;
                    }

                    // 检查后边界
                    if (std::memcmp(boundary + size, MemoryTracker::BOUNDARY_PATTERN, MemoryTracker::BOUNDARY_SIZE) !=
                        0) {
                        return false;
                    }

                    return true;
                }
            }  // namespace boundary
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
