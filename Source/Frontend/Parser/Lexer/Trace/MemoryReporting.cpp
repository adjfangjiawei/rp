#include "MemoryReporting.h"

#include <algorithm>
#include <iostream>

#include "MemoryTrackerCore.h"

namespace rp {
    namespace frontend {
        namespace memory {
            namespace reporting {
                void updateUsageStats(size_t size, bool isAllocation) {
                    if (isAllocation) {
                        currentUsage += size;
                        if (currentUsage > peakUsage) {
                            peakUsage = currentUsage;
                        }
                    } else {
                        currentUsage -= size;
                    }
                }

                void sortLeaksBySize(std::vector<MemoryTracker::AllocationInfo>& leaks) {
                    std::sort(leaks.begin(),
                              leaks.end(),
                              [](const MemoryTracker::AllocationInfo& a, const MemoryTracker::AllocationInfo& b) {
                                  return a.size > b.size;
                              });
                }

                void printAllocationInfo(const MemoryTracker::AllocationInfo& info) {
                    std::cout << "Address: " << info.address << "\n"
                              << "Size: " << info.size << " bytes\n"
                              << "File: " << info.file << "\n"
                              << "Line: " << info.line << "\n"
                              << "Function: " << info.function << "\n"
                              << "Stack trace:\n"
                              << info.stackTrace << std::endl;
                }
            }  // namespace reporting

            MemoryTracker::LeakReport MemoryTracker::generateLeakReport() {
                std::lock_guard<std::mutex> lock(trackerMutex);

                LeakReport report;
                report.totalLeaks = 0;
                report.totalLeakedBytes = 0;

                for (const auto& [ptr, info] : allocations) {
                    if (!info.isDeleted) {
                        report.totalLeaks++;
                        report.totalLeakedBytes += info.size;
                        report.leaks.push_back(info);
                    }
                }

                reporting::sortLeaksBySize(report.leaks);
                return report;
            }

            void MemoryTracker::reset() {
                std::lock_guard<std::mutex> lock(trackerMutex);

                LeakReport report = generateLeakReport();
                if (report.totalLeaks > 0) {
                    std::cerr << "Memory leaks detected during reset!\n"
                              << "Total leaks: " << report.totalLeaks << "\n"
                              << "Total leaked bytes: " << report.totalLeakedBytes << "\n";
                }

                allocations.clear();
                currentUsage = 0;
                peakUsage = 0;
            }
        }  // namespace memory
    }  // namespace frontend
}  // namespace rp
