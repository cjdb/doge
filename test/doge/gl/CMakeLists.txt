add_executable(test.doge.gl.uniform uniform.cpp)
link_core(test.doge.gl.uniform)
target_link_libraries(test.doge.gl.uniform test.main)
add_test(test.uniform test.doge.gl.uniform)

copy_shaders()