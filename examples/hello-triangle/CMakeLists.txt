add_executable(binary_triangle_one_vbo binary_triangle_one_vbo.cpp)
target_link_libraries(binary_triangle_one_vbo doge.gl_core libglfw3.a opengl32.dll)

add_executable(binary_triangle_two_vbo binary_triangle_two_vbo.cpp)
target_link_libraries(binary_triangle_two_vbo doge.gl_core libglfw3.a opengl32.dll)

configure_file(vertex.glsl ${CMAKE_BINARY_DIR}/vertex.glsl COPYONLY)
configure_file(fragment.glsl ${CMAKE_BINARY_DIR}/fragment.glsl COPYONLY)
configure_file(yellow.glsl ${CMAKE_BINARY_DIR}/yellow.glsl COPYONLY)
