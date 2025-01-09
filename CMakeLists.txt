cmake_minimum_required(VERSION 3.16...3.28)

project(NGI VERSION 0.0.1 LANGUAGES CXX C)

find_package(glfw3 3.0 REQUIRED)
find_package(OpenGL REQUIRED)

if (NGI_LOG)
    message(STATUS "Logging is on")
    add_compile_definitions(NGI_LOG=true)
    add_subdirectory(src/log)
endif()

add_subdirectory(deps)
add_subdirectory(src/aux)

# main executable
add_executable(red_triangle)

target_sources(red_triangle PRIVATE src/red_triangle.cpp src/gl/buffer.h src/gl/shader.h
    src/gl/shader_object.cpp src/gl/shader_program.cpp src/gl/vao.cpp) 

target_include_directories(red_triangle PRIVATE SYSTEM deps)

target_include_directories(red_triangle PRIVATE src arrayalgebra)

target_link_libraries(red_triangle PRIVATE ngi_aux glfw OpenGL::GL)

