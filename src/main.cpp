#include "aux/glfw_wrapper.h"

int main()
{
    ngi::glfw::Wrapper wrap{};
    ngi::glfw::Window window{wrap.generate_window(640, 480)};
}
