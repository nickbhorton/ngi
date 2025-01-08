#include "arrayalgebra.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "log/log.h"

#ifdef NGI_LOG
#include "log/log.h"
Log glog{};
#endif

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main()
{
    ngi::glfw::Wrapper wrap{};
    ngi::glfw::Window window{wrap.generate_window(640, 480, key_callback)};

    std::array<aa::vec3, 3> const vertex_positions{
        {{-0.5, -0.5, 0.0}, {0.0, 0.5, 0.0}, {0.5, -0.5, 0.0}}
    };
    ngi::gl::StaticBuffer test(vertex_positions, GL_ARRAY_BUFFER);

    while (!window.should_close()) {
        window.swap();
    }
}
