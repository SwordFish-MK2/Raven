file(GLOB_RECURSE srcs CONFIGURE_DEPENDS src/**/*/cpp src/*.cpp include/**/*.h include/*.h)

add_executable(RavenCore ${srcs})

target_include_directories(RavenCore PUBLIC include)

target_include_directories(RavenCore PUBLIC ${Stb_INCLUDE_DIR})

target_link_libraries(RavenCore PUBLIC tinyobjloader::tinyobjloader)

target_link_libraries(RavenCore PUBLIC tinyxml2::tinyxml2)
