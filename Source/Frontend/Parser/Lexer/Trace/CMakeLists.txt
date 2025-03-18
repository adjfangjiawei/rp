add_library(LexerTrace
    LexerMemoryTracker.cpp
    MemoryTrackerCore.cpp
    MemoryBoundaryChecker.cpp
    MemoryReporting.cpp
    StackTraceCapture.cpp
    MemoryOperators.cpp
    LexerCoverage.cpp
)

target_include_directories(LexerTrace
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
