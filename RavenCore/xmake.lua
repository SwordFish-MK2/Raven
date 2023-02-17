add_includedirs("include")

target("RavenCore")
    add_files("src/**.cpp")

    add_packages("tinyxml2", "tinyobjloader", "stb", "openmp")

    set_rundir("$(projectdir)")
target_end()
