add_subdirectory(gl)
add_subdirectory(utility)

add_library(doge STATIC $<TARGET_OBJECTS:doge.gl.shader_source>
                        $<TARGET_OBJECTS:doge.gl.shader_binary>
                        $<TARGET_OBJECTS:doge.gl.texture>
                        $<TARGET_OBJECTS:doge.utility.file>)
