cmake_minimum_required(VERSION 3.30)
project(RPCompiler VERSION 1.0)

# 设置统一的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)



# 设置包含路径变量
set(RP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用并行编译支持
include(ProcessorCount)
ProcessorCount(N)
if(NOT N EQUAL 0)
    set(CMAKE_BUILD_PARALLEL_LEVEL ${N})
endif()

# 添加子目录
add_subdirectory(Frontend)
# add_subdirectory(Backend)

# # 主程序
# add_executable(rpc main.cpp)
# target_link_libraries(rpc
#     PRIVATE
#     frontend
#     backend
# )
