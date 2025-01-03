#define GLAD_GL_IMPLEMENTATION
#include "glfw_wrapper.h"

int main()
{
    ngi::glfw::Wrapper wrap{};
    ngi::glfw::Window window{wrap.generate_window(640, 480)};
}
