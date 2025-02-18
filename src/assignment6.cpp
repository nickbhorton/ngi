#include "arrayalgebra.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/texture.h"
#include "gl/vao.h"

#include <GLFW/glfw3.h>

int WindowWidth{800};
int WindowHeight{800};

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
}

int main(int argc, char** argv)
{
    ngi::glfw::Wrapper wrap{};
    ngi::glfw::Window window{
        wrap.generate_window(WindowWidth, WindowHeight, key_callback)
    };
    // turn off vsync
    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(
        window.get_window_ptr(),
        framebuffer_size_callback
    );
    glEnable(GL_DEPTH_TEST);

    ngi::gl::ShaderProgram basic_s =
        ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
            {"../res/assignment_shaders/6.vert.glsl", GL_VERTEX_SHADER},
            {"../res/assignment_shaders/6.frag.glsl", GL_FRAGMENT_SHADER}
        });

    ngi::gl::Texture tex1{"../res/images/taylor.jpg"};
    basic_s.update_uniform_vec2f(
        "img_res",
        {static_cast<float>(tex1.size[0]), static_cast<float>(tex1.size[1])}
    );
    basic_s.update_uniform_1f("k", 2.0);
    basic_s.update_uniform_1f("sig", 2.0);
    basic_s.update_uniform_1f("radius", 5.0);

    std::array<aa::vec2, 4> quad_uv{{{0, 0}, {0, 1}, {1, 1}, {1, 0}}};
    std::array<aa::vec3, 4> quad_pos{
        {{-1, -1, 0}, {-1, 1, 0}, {1, 1, 0}, {1, -1, 0}}
    };

    ngi::gl::StaticBuffer<std::array<aa::vec3, 4>> quad_pos_b{
        quad_pos,
        GL_ARRAY_BUFFER
    };
    ngi::gl::StaticBuffer<std::array<aa::vec2, 4>> quad_uv_b{
        quad_uv,
        GL_ARRAY_BUFFER
    };
    ngi::gl::VertexArrayObject quad_vao{};
    quad_vao.set_shader(basic_s);
    quad_vao.attach_buffer_object(quad_pos_b, 0, 3, GL_FLOAT, GL_FALSE, 0);
    quad_vao.attach_buffer_object(quad_uv_b, 1, 2, GL_FLOAT, GL_FALSE, 0);

    while (!window.should_close()) {
        std::array<GLfloat, 4> static constexpr bg_color{0, 0, 1, 1};
        glClearBufferfv(GL_COLOR, 0, bg_color.data());
        glClear(GL_DEPTH_BUFFER_BIT);

        quad_vao.bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, quad_pos.size());

        window.swap();
        glfwPollEvents();
    }
}
