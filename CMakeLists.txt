cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_COMPILER /opt/gcc-15.2/bin/g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 12)
set(CMAKE_LINKER_TYPE MOLD)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(comp LANGUAGES CXX)

option(ENABLE_LEAK_SANITIZER "Enable LeakSanitizer together with AddressSanitizer" OFF)

set(SANITIZER_FLAGS -fsanitize=undefined,address)
if(NOT ENABLE_LEAK_SANITIZER)
    list(APPEND SANITIZER_FLAGS -fno-sanitize=leak)
endif()

set(ACL_PATH "/usr/lib/ac-library/")
include_directories(SYSTEM ${ACL_PATH})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

add_compile_options(
    -pipe
    -Wall -Wextra -Wfloat-equal
    ${SANITIZER_FLAGS} -fno-omit-frame-pointer
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Release>:-march=native>
    $<$<CONFIG:Release>:-mtune=native>
)
add_link_options(${SANITIZER_FLAGS})

add_compile_definitions(
    LOCAL ATCODER
    $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>
)

# Precompiled header (shared across all targets)
add_library(pch OBJECT pch.cpp)
target_precompile_headers(pch PUBLIC <bits/stdc++.h>)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS contests/*.cpp codes/*.cpp heuristic_contests/*.cpp problems/*.cpp)
foreach(path IN LISTS sources)
    get_filename_component(name ${path} NAME_WE)
    get_filename_component(parent_dir ${path} DIRECTORY)
    get_filename_component(parent_name ${parent_dir} NAME)
    get_filename_component(grandparent_dir ${parent_dir} DIRECTORY)
    get_filename_component(grandparent_name ${grandparent_dir} NAME)
    set(target "${grandparent_name}_${parent_name}_${name}")
    add_executable(${target} ${path})
    target_precompile_headers(${target} REUSE_FROM pch)
endforeach()
