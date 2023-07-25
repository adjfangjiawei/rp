if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif (POLICY CMP0048)
project(rp VERSION 1.0.0 LANGUAGES C CXX)
cmake_minimum_required(VERSION 3.22)
include(cmake/CPM.cmake)
# CPMAddPackage(
#   NAME googletest   # 添加依赖库：googletest
#   GITHUB_REPOSITORY google/googletest # GitHub仓库是：google/googletest，CPM会在GitHub网站找到该仓库，并得到对应的仓库链接：https://github.com/google/googletest
#   GIT_TAG release-1.8.1 # Git标签是release-1.8.1
#   VERSION 1.8.1 # 版本号是1.8.1
#   OPTIONS  # 可选参数包括：INSTALL_GTEST：关，gtest_force_shared_crt，开
#       "INSTALL_GTEST OFF"
#       "gtest_force_shared_crt ON"
# )
#简写版
# 通过URI添加依赖库
# CPMAddPackage("uri")
# # 通过URI添加指定版本号的依赖库
# CPMAddPackage("uri@version")
# # 通过URI添加指定标签的依赖库
# CPMAddPackage("uri#tag")
# # 通过URI添加指定版本号和指定标签的依赖库
# CPMAddPackage("uri@version#tag")
# “uri”指网址，例如“https://example.com/my-package-1.2.3.zip”，GitHub、GitLab、BitBucket等开源代码库网址可以使用缩写形式，如下表所示：

# 序号	缩写网址	完整网址
# 1	gh:user/name	https://github.com/user/name.git
# 2	gl:user/name	https://gitlab.com/user/name.git
# 3	bb:user/name	https://bitbucket.org/user/name.git
# ————————————————
# 参考
# 基于URL添加依赖库，版本号依据网址内容推断为：1.2.3
# CPMAddPackage("https://example.com/my-package-1.2.3.zip")
# # 基于URL添加依赖库，版本号依据网址内容推断为：1.2.3，并基于给定的MD5码作依赖库完整性验证
# CPMAddPackage("https://example.com/my-package-1.2.3.zip#MD5=68e20f674a48be3使用8d60e129f600faf7d")
# # 基于URL添加依赖库，版本号显式地指定
# CPMAddPackage("https://example.com/my-package.zip@1.2.3")
# # 基于GitHub网址添加依赖库，依赖库名Catch2，版本号为：2.5.0
# # gh:catchorg/Catch2对应的完整网址为：https://github.com/catchorg/Catch2.git
# CPMAddPackage("gh:catchorg/Catch2@2.5.0")
# # 基于GitHub网址添加依赖库，依赖库名json，版本号为：3.9.1
# # gh:nlohmann/json对应的完整网址为：https://github.com/lohmann/json.git
# CPMAddPackage("gh:nlohmann/json@3.9.1")
set(CMAKE_CXX_STANDARD 20)
#-Werror
add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -std=c++20")
set(CMAKE_BUILD_TYPE Debug)
include_directories(src)
include_directories(ThirdParty/pprint/include)
include_directories(ThirdParty/gtest/include)
link_directories(ThirdParty/gtest/lib)
add_executable(rp src/Type.cpp)
target_link_libraries(rp gtest gtest_main pthread)