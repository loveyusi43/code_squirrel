cmake_minimum_required(VERSION 3.28)
project(squirrel LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(squirrel main.cc)

# 平台特定的编译选项
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC 和 Clang 的编译选项
    target_compile_options(squirrel PRIVATE -Wall -Wextra -Wpedantic -Werror)
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # MSVC (Windows) 的编译选项
    target_compile_options(squirrel PRIVATE 
        /W4     # 最高警告级别
        /WX     # 将警告视为错误
        /permissive- # 标准一致性模式
    )
    
    # 禁用特定的 MSVC 警告（如果需要）
    # target_compile_options(squirrel PRIVATE /wd<warning-number>)
endif()