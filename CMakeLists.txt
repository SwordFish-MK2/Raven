# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
CMAKE_MINIMUM_REQUIRED (VERSION 3.8)
SET(CMAKE_CXX_STANDARD 17)
PROJECT ("Raven")

INCLUDE_DIRECTORIES("C:/Program Files (x86)/Eigen3/include/eigen3" "resources/stb_image")

# 包含子项目。
ADD_SUBDIRECTORY(src bin)