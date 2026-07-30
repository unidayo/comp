cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_COMPILER /opt/gcc-15.2/bin/g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 12)
set(CMAKE_LINKER_TYPE MOLD)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(comp LANGUAGES CXX)

option(ENABLE_SANITIZERS "Enable UndefinedBehaviorSanitizer and AddressSanitizer" ON)
option(ENABLE_LEAK_SANITIZER "Enable LeakSanitizer together with AddressSanitizer" OFF)
option(ENABLE_PCH "Enable a shared precompiled header for C++ targets" ON)
set(COMP_ACTIVE_SOURCE "" CACHE FILEPATH "Build only this source file when set")

set(SANITIZER_FLAGS -fsanitize=undefined,address)
if(NOT ENABLE_LEAK_SANITIZER)
    list(APPEND SANITIZER_FLAGS -fno-sanitize=leak)
endif()

set(ACL_PATH "/usr/lib/ac-library/")
include_directories(${CMAKE_SOURCE_DIR})
include_directories(SYSTEM ${ACL_PATH})

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_C_COMPILER_LAUNCHER   ${CCACHE_PROGRAM})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

add_compile_options(
    -pipe
    -Wall -Wextra -Wfloat-equal
    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Debug>:-gdwarf-4>
    $<$<CONFIG:Debug>:-fno-inline>
    $<$<CONFIG:Debug>:-fno-eliminate-unused-debug-symbols>
    $<$<CONFIG:Debug>:-fvar-tracking>
    $<$<CONFIG:Debug>:-fvar-tracking-assignments>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Release>:-march=native>
    $<$<CONFIG:Release>:-mtune=native>
)
if(ENABLE_SANITIZERS)
    add_compile_options(${SANITIZER_FLAGS} -fno-omit-frame-pointer)
    add_link_options(${SANITIZER_FLAGS})
endif()

add_compile_definitions(
    LOCAL ATCODER
    $<$<CONFIG:Debug>:_GLIBCXX_DEBUG>
)

set(COMP_PCH_HEADERS
    <algorithm>
    <array>
    <bit>
    <bitset>
    <cassert>
    <charconv>
    <cmath>
    <concepts>
    <cstdint>
    <deque>
    <functional>
    <iomanip>
    <iostream>
    <iterator>
    <limits>
    <map>
    <numbers>
    <numeric>
    <optional>
    <queue>
    <random>
    <ranges>
    <set>
    <string>
    <tuple>
    <type_traits>
    <unordered_map>
    <unordered_set>
    <utility>
    <vector>
)

if(ENABLE_PCH)
    # Precompiled header (shared across all targets)
    add_library(pch OBJECT pch.cpp)
    target_precompile_headers(pch PUBLIC ${COMP_PCH_HEADERS})
endif()

if(COMP_ACTIVE_SOURCE)
    cmake_path(ABSOLUTE_PATH COMP_ACTIVE_SOURCE BASE_DIRECTORY "${CMAKE_SOURCE_DIR}" NORMALIZE OUTPUT_VARIABLE active_source)
    cmake_path(IS_PREFIX CMAKE_SOURCE_DIR "${active_source}" NORMALIZE is_workspace_source)
    if(NOT is_workspace_source)
        message(FATAL_ERROR "COMP_ACTIVE_SOURCE must be under ${CMAKE_SOURCE_DIR}: ${COMP_ACTIVE_SOURCE}")
    endif()
    if(NOT active_source MATCHES "\\.cpp$")
        message(FATAL_ERROR "COMP_ACTIVE_SOURCE must be a .cpp file: ${COMP_ACTIVE_SOURCE}")
    endif()
    if(NOT EXISTS "${active_source}")
        message(FATAL_ERROR "COMP_ACTIVE_SOURCE does not exist: ${COMP_ACTIVE_SOURCE}")
    endif()
    set(sources "${active_source}")
else()
    file(GLOB_RECURSE sources CONFIGURE_DEPENDS contests/*.cpp codes/*.cpp heuristic_contests/*.cpp problems/*.cpp)
endif()
foreach(path IN LISTS sources)
    get_filename_component(name ${path} NAME_WE)
    get_filename_component(parent_dir ${path} DIRECTORY)
    get_filename_component(parent_name ${parent_dir} NAME)
    get_filename_component(grandparent_dir ${parent_dir} DIRECTORY)
    get_filename_component(grandparent_name ${grandparent_dir} NAME)
    set(target "${grandparent_name}_${parent_name}_${name}")
    add_executable(${target} ${path})
    if(ENABLE_PCH)
        target_precompile_headers(${target} REUSE_FROM pch)
    endif()
endforeach()
