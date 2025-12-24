set(ACL_PATH "/usr/lib/ac-library/")
set(BITS_STDCPP_PATH "/opt/gcc-15.2/include/c++/15.2.0/x86_64-pc-linux-gnu/bits/stdc++.h")
set(CMAKE_BUILD_PARALLEL_LEVEL 12)
set(CMAKE_LINKER_TYPE MOLD)

cmake_minimum_required(VERSION 4.1.2)
project(comp LANGUAGES CXX)

set(CMAKE_CXX_COMPILER /opt/gcc-15.2/bin/g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")

include_directories(${ACL_PATH})
add_library(pch "pch.cpp")
target_compile_options(pch PUBLIC -O2 -march=native -mtune=native)
target_precompile_headers(pch PUBLIC <bits/stdc++.h> <atcoder/all>)

file(GLOB_RECURSE sources 
contests/*.cpp
codes/*.cpp
)
foreach (path IN LISTS sources)
    get_filename_component(target ${path} NAME_WE)
    add_executable(${target} ${path})
    target_compile_options(${target} PUBLIC -O2 -march=native -mtune=native -Wall -Wextra -Wfloat-equal) 
    target_link_options(${target} PUBLIC)
    target_precompile_headers(${target} REUSE_FROM pch)
endforeach()

add_compile_definitions(LOCAL ATCODER)