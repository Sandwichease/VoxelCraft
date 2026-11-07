
cmake_minimum_required(VERSION 3.16)

project(3Drender LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(triangle
    src/main.cpp
    src/glad.c 
    src/shader.cpp
    src/camera.cpp
    src/chunk.cpp
)

target_include_directories(triangle PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(triangle PRIVATE
    glfw
    OpenGL::GL
)

