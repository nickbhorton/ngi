#include "glfw_wrapper.h"

#include <iostream>

GlfwWrapper::GlfwWrapper() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "glfw failed to initialize\n";
        std::exit(1);
    }
    else {
        std::cout << "glfw initialized successfully\n";
    }
}
GlfwWrapper::~GlfwWrapper() {
    glfwTerminate();
    std::cout << "glfw terminated successfully\n";
}
