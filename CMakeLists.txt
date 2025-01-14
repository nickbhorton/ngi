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
add_subdirectory(src/gl)

# glyph_view
add_executable(glyph_view)

target_sources(glyph_view PRIVATE src/glyph_view.cpp) 

target_include_directories(glyph_view PRIVATE SYSTEM deps)

target_include_directories(glyph_view PRIVATE 
    src 
    arrayalgebra
)

target_link_libraries(glyph_view PRIVATE 
    ngi_gl
    ngi_aux 
    ngi_ttf 
    glfw 
    OpenGL::GL
)

#
add_executable(glyph_sdf)

target_sources(glyph_sdf PRIVATE src/glyph_sdf.cpp) 

target_include_directories(glyph_sdf PRIVATE SYSTEM deps)

target_include_directories(glyph_sdf PRIVATE src arrayalgebra)

target_link_libraries(glyph_sdf PRIVATE ngi_ttf png)

# assignemnt1 
add_executable(assignment1)

target_sources(assignment1 PRIVATE src/assignment1.cpp) 

target_include_directories(assignment1 PRIVATE SYSTEM deps)

target_include_directories(assignment1 PRIVATE 
    src 
    arrayalgebra
)

target_link_libraries(assignment1 PRIVATE 
    ngi_gl
    ngi_aux 
    glfw 
    OpenGL::GL
)
