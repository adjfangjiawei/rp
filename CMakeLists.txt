if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif(POLICY CMP0048)

project(rp VERSION 1.0.0 LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)

# -Werror
add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++20")
set(CMAKE_BUILD_TYPE Debug)
include_directories(ThirdParty/pprint)
file(GLOB_RECURSE rpSource "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
message(rpSource)
add_executable(rp ${rpSource})
target_include_directories(rp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# include(cmake/AddRP.cmake)
# set(ooo "sdvcdjsi")
# # test(ooo)
# message(STATUS ${ooo})

# set(ILike "ds" CACHE STRING "cevc")
# message(STATUS ${ILike})
# message(STATUS ${CMAKE_SYSTEM_NAME})
# find_program(CCACHE_PROGRAM ccache)
# message(STATUS ${CCACHE_PROGRAM})