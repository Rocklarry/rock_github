
# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)


include_directories(wave.h)

SET(CMAKE_CXX_FLAGS "-std=c++0x")

project (Demo1)
# 指定生成目标
add_executable(Demo wave.cpp)
