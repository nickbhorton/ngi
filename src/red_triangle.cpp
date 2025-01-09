#include "arrayalgebra.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/vao.h"
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
    try {
        ngi::glfw::Wrapper wrap{};
        ngi::glfw::Window window{wrap.generate_window(640, 480, key_callback)};

        ngi::gl::ShaderProgram test_s(
            {{"../res/default_shaders/basic.vert.glsl", GL_VERTEX_SHADER},
             {"../res/default_shaders/basic.frag.glsl", GL_FRAGMENT_SHADER}}
        );
        std::array<aa::vec3, 3> const vertex_positions{
            {{-0.5, -0.5, 0.0}, {0.0, 0.5, 0.0}, {0.5, -0.5, 0.0}}
        };
        ngi::gl::StaticBuffer test_b(vertex_positions, GL_ARRAY_BUFFER);
        ngi::gl::VertexArrayObject test_vao{};
        test_vao.attach_shader(test_s);
        test_vao.attach_buffer_object(test_b, 0, 3, GL_FLOAT, GL_FALSE, 0);

        while (!window.should_close()) {
            test_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertex_positions.size());
            window.swap();
        }
    } catch (int& e) {
        std::cout << e << "\n";
    }
}
