# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#

CMAKE_MINIMUM_REQUIRED (VERSION 3.8)
SET(CMAKE_CXX_STANDARD 17)

PROJECT (Raven)

# SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}/Zi")
# SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}/DEBUG /OPT:REF /OPT:ICF")


find_package(tinyxml2 CONFIG REQUIRED)

find_package(Stb REQUIRED)

FIND_PACKAGE(tinyobjloader CONFIG REQUIRED)

add_compile_definitions(RAVEN_PATH="${PROJECT_SOURCE_DIR}")

FIND_PACKAGE(OpenMP REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  message(STATUS "No build type select, defaulting to ${CMAKE_BUILD_TYPE}") 
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions(-D _RAVEN_DEBUG_)
endif()

# 包含子项目。
ADD_SUBDIRECTORY(RavenCore)
