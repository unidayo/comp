set(ACL_PATH "/usr/lib/ac-library/")

cmake_minimum_required(VERSION 3.22)
project(comp CXX)
set(CMAKE_CXX_COMPILER /usr/bin/g++-12)
set(CMAKE_CXX_STANDARD 23)
include_directories(${ACL_PATH})

file(GLOB_RECURSE sources contests/*.cpp)
foreach (path IN LISTS sources)
    get_filename_component(target ${path} NAME_WE)
    add_executable(${target} ${path})
endforeach()

add_compile_options(-O2 
-g 
-mtune=native 
-march=native 
-ftrapv 
-fsanitize=address,undefined 
-fno-omit-frame-pointer 
-fno-sanitize-recover
)

add_compile_definitions(LOCAL)