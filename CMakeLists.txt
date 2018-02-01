# ltcpp -- A compiler front-end project that also supports llvm
#
# Copyright Christopher Di Bella 2017 -- Present
#
#  Use, modification and distribution is subject to the
#  Boost Software License, Version 1.0. (See accompanying
#  file LICENSE_1_0.txt or copy at
#  http://www.boost.org/LICENSE_1_0.txt)
#
# Project home: https://github.com/cjdb/ltcpp.git
#
# "Adapted" from cmcstl2's top-level CMakeLists.txt
#
cmake_minimum_required(VERSION 3.9)

project(doge C CXX)

enable_testing()
include(CTest)

# Compiler flags

## General C++ flags
### Language- and architecture-specific
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fconcepts -march=native")
### Warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-attributes -pedantic -Werror")
### Compiler features
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -flto -flto-odr-type-merging -Wl,-flto")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftemplate-backtrace-limit=0")

## Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fno-inline -fstack-protector-all -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-optimize-sibling-calls")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize=undefined")

## Release flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3 -DNDEBUG")

# Linker flags
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")

#include(CheckIPOSupported)
#check_ipo_supported(RESULT supported OUTPUT error)

# if( supported )
#     message(STATUS "IPO / LTO enabled")
#     set(PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
# else()
#     message(STATUS "IPO / LTO not supported: <${error}>")
# endif()

include_directories(include)

if (NOT ${DOGE_GLFW_PATH} EQUAL "")
   include_directories("${DOGE_GLFW_PATH}/include")
   link_directories("${DOGE_GLFW_PATH}/lib")
endif()

if (NOT ${DOGE_GLM_PATH} EQUAL "")
   include_directories("${DOGE_GLM_PATH}/include")
endif()

if (NOT ${DOGE_GLI_PATH} EQUAL "")
   include_directories("${DOGE_GLI_PATH}/include")
endif()

if (NOT ${DOGE_CMCSTL2_PATH} EQUAL "")
   include_directories("${DOGE_CMCSTL2_PATH}/include")
endif()

if (NOT ${DOGE_GSL_PATH} EQUAL "")
   include_directories("${DOGE_GSL_PATH}/include")
endif()

if (NOT ${DOGE_STB_IMAGE_PATH} EQUAL "")
   include_directories("${DOGE_STB_IMAGE_PATH}")
endif()

if (NOT ${DOGE_CATCH2_PATH} EQUAL "")
   include_directories("${DOGE_CATCH2_PATH}")
endif()

add_subdirectory(source)

function(copy_shaders)
   file(GLOB files "*.glsl")
   foreach(file ${files})
      configure_file(${file} ${CMAKE_BINARY_DIR} COPYONLY)
      configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
   endforeach()
endfunction(copy_shaders)

function(link_core object)
   target_link_libraries(${object} doge gl_core)
   if (WIN32)
      target_link_libraries(${object} libglfw3.a opengl32)
   else()
      target_link_libraries(${object} glfw3 GL rt m dl Xrandr Xrender Xi Xext Xfixes X11 pthread xcb Xau Xdmcp)
   endif()
endfunction(link_core)

add_subdirectory(examples)
add_subdirectory(test)

install(DIRECTORY $(PROJECT_SOURCE_DIR)/include DESTINATION include)
