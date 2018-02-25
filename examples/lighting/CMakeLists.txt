add_executable(lighting.example lighting.cpp)
link_core(lighting.example)

add_subdirectory(directional_light)
add_subdirectory(point_light)

copy_shaders()
