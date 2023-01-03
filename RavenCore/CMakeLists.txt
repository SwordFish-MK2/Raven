file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/**/*/cpp src/*.cpp include/**/*.h include/*.h)

add_executable(RavenCore ${srcs})

target_include_directories(RavenCore PRIVATE include)

target_include_directories(RavenCore PRIVATE ${STB_INCLUDE_DIRS})

target_link_libraries(RavenCore PRIVATE tinyobjloader::tinyobjloader)

target_link_libraries(RavenCore PRIVATE tinyxml2::tinyxml2)
