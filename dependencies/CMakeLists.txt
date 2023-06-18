add_subdirectory(glfw)

set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
set(IM_PLOT_DIR ${CMAKE_CURRENT_LIST_DIR}/implot)
set(IMGUI_SOURCE_FILES ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
set(IM_PLOT_BUILD_INCLUDE_HEADERS $<BUILD_INTERFACE:${IM_PLOT_DIR}>)

file(GLOB_RECURSE IM_PLOT_SOURCE_FILES ${IM_PLOT_DIR}/*.cpp)


find_package(Vulkan REQUIRED)
set(LIBRARIES "glfw;Vulkan::Vulkan")

add_library(imgui ${IMGUI_SOURCE_FILES} ${IM_PLOT_SOURCE_FILES})
target_link_libraries(imgui PUBLIC ${LIBRARIES})
target_include_directories(imgui PUBLIC
        $<BUILD_INTERFACE:${IMGUI_DIR}>
        ${IM_PLOT_BUILD_INCLUDE_HEADERS}
        $<INSTALL_INTERFACE:./>

        )