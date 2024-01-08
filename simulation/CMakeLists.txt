cmake_minimum_required(VERSION 3.15)

set(CRC_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(IMGUI_SRCS
        src/imgui/imgui.cpp
        src/imgui/imgui_draw.cpp
        src/imgui/imgui_widgets.cpp
        src/imgui/imgui_demo.cpp
        src/imgui/imgui_impl_glfw.cpp
        src/imgui/imgui_impl_opengl3.cpp
)

project(Simulator)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(src/imgui)
add_subdirectory(test)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Simulator 
        src/main.cpp 
        src/app.cpp
        src/reservoir.cpp
        ${CRC_SRC_DIR}/pid/pid.cpp
)

target_include_directories(Simulator PRIVATE 
        ${CRC_SRC_DIR}
        src/imgui
)

target_link_libraries(Simulator PRIVATE glfw imgui OpenGL::GL)