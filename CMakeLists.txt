cmake_minimum_required(VERSION 3.20)
project(Lens_Engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(Lens_Engine
    src/main.cpp
)

target_include_directories(Lens_Engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/camera
    ${CMAKE_CURRENT_SOURCE_DIR}/color
    ${CMAKE_CURRENT_SOURCE_DIR}/materials
    ${CMAKE_CURRENT_SOURCE_DIR}/math_objects
    ${CMAKE_CURRENT_SOURCE_DIR}/objects
    ${CMAKE_CURRENT_SOURCE_DIR}/obj_files
    ${CMAKE_CURRENT_SOURCE_DIR}/optics

)
