add_library(ngi_gl)

target_sources(ngi_gl PRIVATE 
    buffer.h 
    shader.h
    shader_object.cpp 
    shader_program.cpp 
    vao.cpp
    texture.h
    texture.cpp
) 

target_include_directories(ngi_gl PUBLIC 
    ${CMAKE_SOURCE_DIR}/src 
    ${CMAKE_SOURCE_DIR}/arrayalgebra
)

target_link_libraries(ngi_gl PUBLIC 
    ngi_glad 
    glfw 
    OpenGL::GL
)
