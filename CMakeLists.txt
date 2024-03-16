set(ACL_PATH "/usr/lib/ac-library/")
set(BITS_STDCPP_PATH "/usr/include/x86_64-linux-gnu/c++/12/bits/stdc++.h")

cmake_minimum_required(VERSION 3.22)
project(comp CXX)

set(CMAKE_CXX_COMPILER /usr/bin/g++-12)
set(CMAKE_CXX_STANDARD 23)
include_directories(${ACL_PATH})

add_library(pch "pch.cpp")
target_compile_options(pch PUBLIC -O2)
target_precompile_headers(pch PUBLIC <bits/stdc++.h> <atcoder/all>)

file(GLOB_RECURSE sources contests/*.cpp)
foreach (path IN LISTS sources)
    get_filename_component(target ${path} NAME_WE)
    add_executable(${target} ${path})
    target_compile_options(${target} PUBLIC -O2)
    target_link_options(${target} PUBLIC -O2)
    target_precompile_headers(${target} REUSE_FROM pch)
endforeach()

add_compile_definitions(LOCAL)