add_executable(lighting.example lighting.cpp)
link_core(lighting.example)

add_executable(example.entity.directional_light directional_light.cpp)
link_core(example.entity.directional_light)

copy_shaders()
