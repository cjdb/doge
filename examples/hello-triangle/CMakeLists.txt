add_executable(binary_triangle_one_vbo binary_triangle_one_vbo.cpp)
link_core(binary_triangle_one_vbo)

add_executable(binary_triangle_two_vbo binary_triangle_two_vbo.cpp)
link_core(binary_triangle_two_vbo)

configure_file(vertex.glsl ${CMAKE_BINARY_DIR}/vertex.glsl COPYONLY)
configure_file(fragment.glsl ${CMAKE_BINARY_DIR}/fragment.glsl COPYONLY)
configure_file(yellow.glsl ${CMAKE_BINARY_DIR}/yellow.glsl COPYONLY)
