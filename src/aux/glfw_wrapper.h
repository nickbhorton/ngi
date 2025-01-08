#pragma once

// ensures that glfw3.h does not include OpenGL headers
#include <glad/gl.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ngi::glfw
{

class Window
{
    GLFWwindow* window;
    int width;
    int height;
    bool destroy_responsibility;

public:
    explicit Window(GLFWwindow* window, int width, int height);
    ~Window();
    Window(const Window& other) = delete;
    Window& operator=(const Window& other) = delete;
    Window(Window&& other) noexcept;
    Window& operator=(Window&& other) noexcept;

    bool should_close();

    GLFWwindow* get_window_ptr();
    int get_width();
    int get_height();
    void swap();
};

class Wrapper
{
public:
    Wrapper();
    ~Wrapper();
    Wrapper(const Wrapper& other) = delete;
    Wrapper& operator=(const Wrapper& other) = delete;
    Wrapper(Wrapper&& other) noexcept = delete;
    Wrapper& operator=(Wrapper&& other) noexcept = delete;

    Window generate_window(int width, int height, GLFWkeyfun callback);
};

} // namespace ngi::glfw
