add_library(imgui.core OBJECT imgui.cpp)
add_library(imgui.draw OBJECT imgui_draw.cpp)
add_library(imgui STATIC $<TARGET_OBJECTS:imgui.draw>
                         $<TARGET_OBJECTS:imgui.core>)