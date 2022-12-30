file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/**/*/cpp src/*.cpp include/**/*.h include/*.h)

add_executable(Raven ${srcs})

target_include_directories(Raven PUBLIC include)

target_include_directories(Raven PUBLIC ${STB_INCLUDE_DIRS})

target_link_libraries(Raven PUBLIC tinyobjloader::tinyobjloader)

target_link_libraries(Raven PUBLIC tinyxml2::tinyxml2)
