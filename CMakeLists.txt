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

# assignemnt7
add_executable(assignment7)

target_sources(assignment7 PRIVATE src/assignment7.cpp) 

target_link_libraries(assignment7 PRIVATE 
    ngi_gl
    ngi_aux 
    ngi_imgui
)

# minecraft
add_executable(minecraft)

target_sources(minecraft PRIVATE src/minecraft.cpp) 

target_link_libraries(minecraft PRIVATE 
    ngi_gl
    ngi_aux 
    ngi_imgui
)
