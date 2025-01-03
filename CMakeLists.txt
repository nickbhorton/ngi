cmake_minimum_required(VERSION 3.16...3.28)

project(NGI VERSION 0.0.1 LANGUAGES CXX)

find_package(glfw3 3.0 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(main src/main.cpp src/glfw_wrapper.cpp)

target_include_directories(main PRIVATE SYSTEM deps)

target_link_libraries(main PRIVATE glfw OpenGL::GL)
