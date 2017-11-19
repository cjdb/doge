add_library(gl_core OBJECT gl_core.cpp)
add_library(doge.gl_core STATIC $<TARGET_OBJECTS:gl_core>)
