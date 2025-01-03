#include "glfw_wrapper.h"

#include <iostream>

using namespace ngi::glfw;

void error_callback(int error, const char* desc)
{
    std::cerr << "glfw Error(" << error << "): " << desc << "\n";
}

Wrapper::Wrapper()
{
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "glfw failed to initialize\n";
        std::exit(1);
    }
    std::cout << "glfw initialized successfully\n";
    glfwSetErrorCallback(error_callback);
}
Wrapper::~Wrapper()
{
    glfwTerminate();
    std::cout << "glfw terminated successfully\n";
}

Window Wrapper::generate_window(int width, int height)
{
    Window w{};
    w.width = width;
    w.height = height;
    w.window = glfwCreateWindow(width, height, "ngi window", NULL, NULL);
    if (!w.window) {
        std::cerr << "window failed to initialize\n";
        std::exit(1);
    }
    glfwMakeContextCurrent(w.window);
    gladLoadGL(glfwGetProcAddress);
    return w;
}

Window::~Window()
{
    glfwDestroyWindow(this->window);
    std::cout << "glfwWindow destroyed successfully\n";
}
