add_executable(lighting.example lighting.cpp)
link_core(lighting.example)

add_subdirectory(directional_light)
add_subdirectory(point_light)
add_subdirectory(spotlight)

copy_shaders()
