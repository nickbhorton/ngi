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
add_subdirectory(src/aux/font)

# main executable
add_executable(glyph_view)

target_sources(glyph_view PRIVATE src/glyph_view.cpp src/gl/buffer.h src/gl/shader.h
    src/gl/shader_object.cpp src/gl/shader_program.cpp src/gl/vao.cpp) 

target_include_directories(glyph_view PRIVATE SYSTEM deps)

target_include_directories(glyph_view PRIVATE src arrayalgebra)

target_link_libraries(glyph_view PRIVATE ngi_aux ngi_ttf glfw OpenGL::GL)

