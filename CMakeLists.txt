cmake_minimum_required(VERSION 3.20)
project(vui)


set(CMAKE_CXX_STANDARD 17)

option(BUILD_VUI_TEST "vui test" ON)

add_subdirectory(dependencies)
add_subdirectory(src)

if(${BUILD_VUI_TEST})
    add_subdirectory(test)
endif()