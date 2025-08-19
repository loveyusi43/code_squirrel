cmake_minimum_required(VERSION 3.28)
project(squirrel LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(squirrel main.cc)

target_compile_options(squirrel PRIVATE -Wall -Wextra -Wpedantic -Werror)