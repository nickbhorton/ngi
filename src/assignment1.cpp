// This is shader golf. Write the shortest shader possible

#include "arrayalgebra.h"
#include "aux/common_objects.h"
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
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(int argc, char** argv)
{
    // try catch abstraction because throwing ensures destruction of all
    // objects.
    try {
        ngi::glfw::Wrapper wrap{};
        ngi::glfw::Window window{wrap.generate_window(800, 800, key_callback)};

        ngi::gl::ShaderProgram test_s(
            {{"../res/default_shaders/basic.vert.glsl", GL_VERTEX_SHADER},
             {"../res/default_shaders/basic.frag.glsl", GL_FRAGMENT_SHADER}}
        );
        test_s.update_uniform_vec4f("color", {1.0, 1.0, 1.0, 1.0});

        std::array<aa::vec3, 36> cube{ngi::common_obj::cube};
        for (auto& v : cube) {
            v = 0.5f * v;
        }
        ngi::gl::StaticBuffer<std::array<aa::vec3, 36>> cube_b{
            cube,
            GL_ARRAY_BUFFER
        };
        ngi::gl::VertexArrayObject cube_vao{};
        cube_vao.attach_shader(test_s);
        cube_vao.attach_buffer_object(cube_b, 0, 3, GL_FLOAT, GL_FALSE, 0);

        while (!window.should_close()) {
            cube_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, cube.size());
            window.swap();
        }
    } catch (int& e) {
        std::cout << "integer error thrown: " << e << "\n";
    }
}
