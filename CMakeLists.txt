# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
project ("Raven")

include_directories("C:/Program Files (x86)/Eigen3/include/eigen3")

# 包含子项目。
add_subdirectory(src bin)