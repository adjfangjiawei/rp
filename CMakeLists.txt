cmake_minimum_required(VERSION 3.10)
project(rp)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 设置包含路径变量
set(RP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Source)

# 添加子目录
add_subdirectory(Source)
add_subdirectory(Example)

# 设置示例程序的输出目录
set_target_properties(
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/example/bin"
)
