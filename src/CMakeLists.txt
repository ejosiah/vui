#include_directories(../dependencies/glfw/include)
#include_directories(../dependencies/imgui ../dependencies/imgui/backends)
include_directories(../dependencies/imgui/backends)

file(GLOB_RECURSE HEADER_FILES *.h *.hpp)
file(GLOB_RECURSE SOURCE_FILES *.c *.cpp)

add_library(vui ${HEADER_FILES} ${SOURCE_FILES} ${IMGUI_SOURCE_FILES})
target_link_libraries(vui PUBLIC imgui)

target_include_directories(vui PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:/include>
)