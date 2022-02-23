include_directories("C:/Program Files (x86)/Eigen3/include/eigen3")

add_executable(Raven "Raven.cpp" "core/math.h" "core/base.h" "core/math.cpp" "core/transform.h" 
"core/transform.cpp" "core/ray.h" "core/aabb.h" "core/interaction.h" 
"core/interaction.cpp" "core/camera.h" "camera/projectiveCamera.h" "camera/projectiveCamera.cpp" "core/film.h" "core/film.cpp" 
"core/renderer.h" "core/renderer.cpp" "renderer/simplified.h" "renderer/simplified.cpp" "core/scene.h"
"core/texture.h" "core/scene.cpp" "core/primitive.h" "core/shape.h" "core/material.h" "core/light.h"
"core/primitive.cpp" "shape/sphere.h" "shape/sphere.cpp" "core/distribution.h" "material/matte.h" "material/matte.cpp"
"shading_model/lambertain.h" "shading_model/lambertain.cpp"  "core/bxdf.h" "core/bsdf.h" "shading_model/oren_nayar.h" 
"shading_model/oren_nayar.cpp")
INCLUDE_DIRECTORIES(../resource/stb_image)