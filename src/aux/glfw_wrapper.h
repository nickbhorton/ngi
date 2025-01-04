#pragma once

// ensures that glfw3.h does not include OpenGL headers
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ngi::glfw
{

struct Window {
    GLFWwindow* window;
    int width;
    int height;
    ~Window();
};

class Wrapper
{
public:
    Wrapper();
    ~Wrapper();

    Window generate_window(int width, int height);
};

} // namespace ngi::glfw
