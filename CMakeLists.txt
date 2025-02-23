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

target_link_libraries(glyph_view PRIVATE 
    ngi_aux 
    ngi_gl
    ngi_ttf 
)

# glyph_sdf
add_executable(glyph_sdf)

target_sources(glyph_sdf PRIVATE src/glyph_sdf.cpp) 

target_include_directories(glyph_sdf PRIVATE src arrayalgebra)

target_link_libraries(glyph_sdf PRIVATE ngi_ttf png)

# assignemnt1 
add_executable(assignment1)

target_sources(assignment1 PRIVATE src/assignment1.cpp) 

target_link_libraries(assignment1 PRIVATE 
    ngi_gl
    ngi_aux 
)

# assignemnt2
add_executable(assignment2)

target_sources(assignment2 PRIVATE src/assignment2.cpp) 

target_link_libraries(assignment2 PRIVATE 
    ngi_gl
    ngi_aux 
)

# assignment3
add_executable(assignment3)

target_sources(assignment3 PRIVATE src/assignment3.cpp) 

target_link_libraries(assignment3 PRIVATE 
    ngi_gl
    ngi_aux 
)


# assignment4
add_executable(assignment4)

target_sources(assignment4 PRIVATE src/assignment4.cpp) 

target_link_libraries(assignment4 PRIVATE 
    glfw 
    vulkan
)

# assignment6
add_executable(assignment6)

target_sources(assignment6 PRIVATE src/assignment6.cpp) 

target_link_libraries(assignment6 PRIVATE 
    ngi_gl
    ngi_aux 
    ngi_imgui
)

# assignemnt7
add_executable(assignment7)

target_sources(assignment7 PRIVATE src/assignment7.cpp) 

target_link_libraries(assignment7 PRIVATE 
    ngi_gl
    ngi_aux 
    ngi_imgui
)
