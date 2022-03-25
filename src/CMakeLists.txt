SET(RAVEN_HEADER_ALL)
FILE(GLOB_RECURSE RAVEN_HEADER_ALL	"./core/*.h"
										"./accelerate/*.h" 
										"./camera/*.h" 
										"./material/*.h" 
										"./renderer/*.h" 
										"./shading_model/*.h"
										"./shape/*.h"
										"./light/*.h"
										"./filter.*.h"
										"./utils/*.h"
										"./texture/*.h")
SET(RAVEN_SOURCE_ALL)
FILE(GLOB_RECURSE RAVEN_SOURCE_ALL	"./core/*.cpp"
										"./accelerate/*.cpp" 
										"./camera/*.cpp" 
										"./material/*.cpp" 
										"./renderer/*.cpp" 
										"./shading_model/*.cpp"
										"./shape/*.cpp"
										"./light/*.cpp"
										"./filter/*.cpp"
										"./utils/*.cpp"
										"./texture/*.cpp")

ADD_EXECUTABLE(Raven "Raven.h" "Raven.cpp" ${RAVEN_SOURCE_ALL} ${RAVEN_HEADER_ALL})

