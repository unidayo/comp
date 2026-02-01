set(ACL_PATH "/usr/lib/ac-library/")
set(BITS_STDCPP_PATH "/opt/gcc-15.2/include/c++/15.2.0/x86_64-pc-linux-gnu/bits/stdc++.h")
set(CMAKE_BUILD_PARALLEL_LEVEL 12)
set(CMAKE_LINKER_TYPE MOLD)

# find_program(CCACHE_PROGRAM ccache)
# if(CCACHE_PROGRAM)
#     set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
#     set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
# endif()

cmake_minimum_required(VERSION 4.1.2)
project(comp LANGUAGES CXX)

set(CMAKE_CXX_COMPILER /opt/gcc-15.2/bin/g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${ACL_PATH})
add_library(pch "pch.cpp")
target_compile_options(pch PUBLIC
    -Wall -Wextra -Wfloat-equal
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:Release>:-O2 -march=native -mtune=native>
)
target_precompile_headers(pch PUBLIC <bits/stdc++.h> <atcoder/all> <ankerl/unordered_dense.h>)

file(GLOB_RECURSE sources 
contests/*.cpp
codes/*.cpp
)
foreach (path IN LISTS sources)
    get_filename_component(target ${path} NAME_WE)
    add_executable(${target} ${path})
    target_compile_options(${target} PUBLIC
        -Wall -Wextra -Wfloat-equal
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:Release>:-O2 -march=native -mtune=native>
    )
    target_link_options(${target} PUBLIC)
    target_precompile_headers(${target} REUSE_FROM pch)
endforeach()

add_compile_definitions(LOCAL ATCODER)