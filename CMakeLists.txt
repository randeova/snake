# ============================================================================
#  贪吃蛇 · 构建脚本（CMakeLists.txt）
#
#  这个文件里没有一行游戏逻辑。它只回答一个问题：
#      "把哪些 .cpp 文件、按什么规则，编译成一个 .exe？"
#
#  你可以把它当成"写给编译器的说明书"。
# ============================================================================

# 要求 CMake 版本不低于 3.20（低于这个版本会直接报错，而不是编出奇怪的结果）
cmake_minimum_required(VERSION 3.20)

# 项目自我介绍：名字 snake，版本 0.1.0，主要语言 C++
project(snake
    VERSION 0.1.0
    DESCRIPTION "Console Snake game - a learning project"
    LANGUAGES CXX
)

# 使用 C++20 标准。想改用 C++17 就只改下面这一个数字，其它都不用动。
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ninja / Makefile 这类"单配置"生成器必须显式指定构建类型，这里默认给 Debug。
# （Visual Studio 这类"多配置"生成器不需要，因为它在界面上让你选 Debug/Release。）
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ★ 最重要的一段：要编译的源文件清单。
#   以后每新增一个 .cpp，都要把它加到这里的括号中，
#   否则那个文件就像不存在一样 —— 你会看到
#   error LNK2019 / LNK2001: 无法解析的外部符号。
add_executable(snake
    src/main.cpp
    src/game.cpp
    src/render.cpp
    src/input.cpp
)

# ★ 让 #include "snake/game.hpp" 这种写法能找到 include 目录。
#   没有这一行，编译器会说"找不到源文件 snake/game.hpp"。
target_include_directories(snake PRIVATE include)

# ★ 第二重要的一行：打开所有能开的警告。
#   新手阶段，编译器的一句警告往往比任何教程都值钱。
if(MSVC)
    target_compile_options(snake PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(snake PRIVATE -Wall -Wextra -Wpedantic)
endif()
