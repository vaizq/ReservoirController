cmake_minimum_required(VERSION 3.6)

add_library(
        imgui STATIC
        imgui_demo.cpp
        imgui_draw.cpp
        imgui_impl_glfw.cpp
        imgui_impl_opengl3.cpp
        imgui_stdlib.cpp
        imgui_tables.cpp
        imgui_widgets.cpp
        imgui.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})