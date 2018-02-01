add_executable(test.doge.gl.uniform uniform.cpp)
link_core(test.doge.gl.uniform)
target_link_libraries(test.doge.gl.uniform test.main)
copy_shaders()

add_test(test.uniform test.doge.gl.uniform)