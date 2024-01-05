# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

FILE(GLOB_RECURSE app_sources 
    ${SRC_DIR}/*.* 
    ${SRC_DIR}/core/*.*
    ${SRC_DIR}/ph/*.*
    ${SRC_DIR}/ec/*.*
    ${SRC_DIR}/liquid_level/*.*
    ${SRC_DIR}/platform/proto/*.*
)

idf_component_register(SRCS ${app_sources})
