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
cmake_minimum_required(VERSION 3.6)

project(doge C CXX)

enable_testing()
include(CTest)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fconcepts -ftemplate-backtrace-limit=0 -fmax-errors=3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-attributes -pedantic -Werror -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fno-inline -fstack-protector-all -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-optimize-sibling-calls -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g0 -DNDEBUG")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g3 -DNDEBUG")

include_directories(include)

if (NOT ${DOGE_GLFW_PATH} EQUAL "")
   include_directories(${DOGE_GLFW_PATH}/include)
   link_directories(${DOGE_GLFW_PATH}/lib)
endif()

if (NOT ${DOGE_GLM_PATH} EQUAL "")
   include_directories(${DOGE_GLM_PATH}/include)
endif()

if (NOT ${DOGE_CMCSTL2_PATH} EQUAL "")
   include_directories(${DOGE_CMCSTL2_PATH}/include)
endif()

add_subdirectory(source)
add_subdirectory(examples)
#add_subdirectory(test)

install(DIRECTORY $(PROJECT_SOURCE_DIR)/include DESTINATION include)
