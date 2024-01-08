
set(CMAKE_CXX_STANDARD 23)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src)

find_package(Catch2 REQUIRED)

add_executable(tests
    test_main.cpp
    ${SRC_DIR}/reservoir.cpp
)

target_include_directories(tests PRIVATE ${SRC_DIR})

target_link_libraries(tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)