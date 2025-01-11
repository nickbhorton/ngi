#include "arrayalgebra.h"
#include "aux/font/ttf.h"
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

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "enter a glyph index\n";
        std::exit(1);
    }
    // try catch abstraction because throwing ensures destruction of all
    // objects. This is primarily important for the log class which outputs
    // somewhere on its destruction.
    try {
        std::string font_filename{"/usr/share/fonts/TTF/FiraCode-Regular.ttf"};
        auto glyphs{ngi::font::get_simple_glyphs(font_filename)};
        size_t glyph_index{atoi(argv[1]) % glyphs.size()};
        auto empty_glyph = ngi::font::get_contours(glyphs[glyph_index]);

        ngi::glfw::Wrapper wrap{};
        ngi::glfw::Window window{wrap.generate_window(640, 480, key_callback)};

        ngi::gl::ShaderProgram test_s(
            {{"../res/default_shaders/basic.vert.glsl", GL_VERTEX_SHADER},
             {"../res/default_shaders/basic.frag.glsl", GL_FRAGMENT_SHADER}}
        );

        float max_x{static_cast<float>(
            glyphs[0].gc.x_max - glyphs[glyph_index].gc.x_min
        )};
        float max_y{static_cast<float>(
            glyphs[0].gc.y_max - glyphs[glyph_index].gc.y_min
        )};
        std::vector<ngi::gl::VertexArrayObject> vaos{};
        std::vector<ngi::gl::StaticBuffer<aa::vec3>> buffers{};
        std::cout << "contour count: " << empty_glyph.size() << "\n";
        for (auto const& c : empty_glyph) {
            std::vector<aa::vec3> vertex_positions{};
            for (size_t i = 0; i < c.size(); i++) {
                auto const& [x, y] = std::get<0>(c.at(i));
                vertex_positions.push_back(
                    {(static_cast<float>(x) / max_x) - 0.5f,
                     (static_cast<float>(y) / max_y) - 0.5f,
                     0.0}
                );
            }
            ngi::gl::StaticBuffer test_b(vertex_positions, GL_ARRAY_BUFFER);
            ngi::gl::VertexArrayObject test_vao{};
            test_vao.attach_shader(test_s);
            test_vao.attach_buffer_object(test_b, 0, 3, GL_FLOAT, GL_FALSE, 0);
            vaos.push_back(std::move(test_vao));
            buffers.push_back(std::move(test_b));
        }

        while (!window.should_close()) {
            for (auto i = 0; i < vaos.size(); i++) {
                vaos[i].bind();
                glDrawArrays(
                    GL_LINE_LOOP,
                    0,
                    buffers[i].byte_count() / sizeof(aa::vec3)
                );
            }
            window.swap();
        }
    } catch (int& e) {
        std::cout << "error throw: " << e << "\n";
    }
}
