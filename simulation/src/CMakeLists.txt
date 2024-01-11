find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(Simulator 
        main.cpp 
        simulation.cpp
        app_base.cpp
        reservoir.cpp
        ${CRC_SRC_DIR}/pid/pid.cpp
)

target_include_directories(Simulator PRIVATE 
        ${CRC_SRC_DIR}
)

target_link_libraries(Simulator PRIVATE glfw imgui OpenGL::GL)