add_subdirectory(gl)
add_subdirectory(utility)

add_library(doge STATIC $<TARGET_OBJECTS:doge.gl.shader_source>
                        $<TARGET_OBJECTS:doge.gl.shader_binary>
                        $<TARGET_OBJECTS:doge.gl.texture>
                        $<TARGET_OBJECTS:doge.utility.file>)

if (GIT_FOUND)
   ExternalProject_Add(
      scope_guard
      GIT_REPOSITORY https://github.com/PeterSommerlad/SC22WG21_Papers.git
      SOURCE_DIR "${CMAKE_BINARY_DIR}/scope_guard"
      CONFIGURE_COMMAND ""
      BUILD_COMMAND     ""
      INSTALL_COMMAND   ""
      TEST_COMMAND ""
      LOG_DOWNLOAD 1
      UPDATE_DISCONNECTED 1)

   add_dependencies(doge scope_guard)
else()
   message(FATAL_ERROR "Could not find git.")
endif()