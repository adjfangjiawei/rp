# Create test targets for lexer components

# Unit tests
add_executable(LexerTests
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTest.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTest.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTestEdgeCases.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTestImpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTestRecovery.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTestUnicode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerTestUtils.h
)

find_package(GTest REQUIRED)

target_link_libraries(LexerTests
    PRIVATE
    MainLexer
    LexerLiterals
    LexerToken
    TokenLexer
    LexerErrorRecovery
    LexerTrace
    LexerUtils
    Diagnostic
    LexerUnicode
    GTest::gtest
    GTest::gtest_main
)

# Benchmark tests
add_executable(LexerBenchmarks
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerBenchmark.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerBenchmark.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerPerformanceTest.cpp
)

find_package(benchmark REQUIRED)

target_link_libraries(LexerBenchmarks
    PRIVATE
    MainLexer
    LexerLiterals
    LexerToken
    TokenLexer
    LexerErrorRecovery
    LexerTrace
    LexerUtils
    Diagnostic
    LexerUnicode
    benchmark::benchmark
    benchmark::benchmark_main
)

# Coverage tests
add_executable(LexerCoverage
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerCoverage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerCoverage.h
    ${CMAKE_CURRENT_SOURCE_DIR}/LexerRegressionTest.h
)

target_link_libraries(LexerCoverage
    PRIVATE
    MainLexer
    LexerLiterals
    LexerToken
    TokenLexer
    LexerErrorRecovery
    LexerTrace
    LexerUtils
    Diagnostic
    GTest::gtest
    GTest::gtest_main
    LexerUnicode
)

# Set include directories for all test targets
target_include_directories(LexerTests
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RP_INCLUDE_DIR}
)

target_include_directories(LexerBenchmarks
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RP_INCLUDE_DIR}
)

target_include_directories(LexerCoverage
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${RP_INCLUDE_DIR}
)

# Register tests
add_test(NAME LexerTests COMMAND LexerTests)
add_test(NAME LexerBenchmarks COMMAND LexerBenchmarks)
add_test(NAME LexerCoverage COMMAND LexerCoverage)
