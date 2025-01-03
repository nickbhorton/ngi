#pragma once

// ensures that glfw3.h does not include OpenGL headers
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/gl.h>

class GlfwWrapper {
public:
    GlfwWrapper();
    ~GlfwWrapper();
};
