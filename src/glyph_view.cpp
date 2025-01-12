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
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
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

        std::cout << "contour count: "
                  << glyphs[glyph_index].gc.number_of_contours << "\n";
        std::cout << "coord count: " << glyphs[glyph_index].x_coords.size()
                  << "\n";

        auto glyph_contours = ngi::font::get_contours(glyphs[glyph_index]);

        ngi::glfw::Wrapper wrap{};
        ngi::glfw::Window window{wrap.generate_window(800, 800, key_callback)};

        ngi::gl::ShaderProgram test_s(
            {{"../res/default_shaders/basic.vert.glsl", GL_VERTEX_SHADER},
             {"../res/default_shaders/basic.frag.glsl", GL_FRAGMENT_SHADER}}
        );

        float x_min{static_cast<float>(glyphs[glyph_index].gc.x_min)};
        float x_max{static_cast<float>(glyphs[glyph_index].gc.x_max)};
        float y_min{static_cast<float>(glyphs[glyph_index].gc.y_min)};
        float y_max{static_cast<float>(glyphs[glyph_index].gc.y_max)};

        std::vector<ngi::gl::VertexArrayObject> vaos{};
        std::vector<ngi::gl::StaticBuffer<aa::vec3>> buffers{};
        std::vector<aa::vec4> colors{};
        // bounding box
        {
            std::vector<aa::vec3> vertex_positions{};
            vertex_positions.push_back({0.5, 0.5, 0.0});
            vertex_positions.push_back({0.5, -0.5, 0.0});
            vertex_positions.push_back({-0.5, -0.5, 0.0});
            vertex_positions.push_back({-0.5, 0.5, 0.0});
            ngi::gl::StaticBuffer test_b(vertex_positions, GL_ARRAY_BUFFER);
            ngi::gl::VertexArrayObject test_vao{};
            test_vao.attach_shader(test_s);
            test_vao.attach_buffer_object(test_b, 0, 3, GL_FLOAT, GL_FALSE, 0);
            vaos.push_back(std::move(test_vao));
            buffers.push_back(std::move(test_b));
            colors.push_back({1.0, 1.0, 1.0, 0.5});
        }
        for (auto i = 0; i < glyph_contours.size(); i++) {
            std::vector<aa::vec3> vertex_positions{};
            for (size_t j = 0; j < glyph_contours[i].size(); j += 2) {
                // bezier interpolation
                auto const& [x1, y1] = std::get<0>(glyph_contours[i].at(j));
                auto const& [x2, y2] = std::get<0>(
                    glyph_contours[i].at((j + 1) % glyph_contours[i].size())
                );
                auto const& [x3, y3] = std::get<0>(
                    glyph_contours[i].at((j + 2) % glyph_contours[i].size())
                );
                aa::vec3 p1{
                    ((static_cast<float>(x1) - x_min) / (x_max - x_min)) - 0.5f,
                    ((static_cast<float>(y1) - y_min) / (y_max - y_min)) - 0.5f,
                    0.0
                };
                aa::vec3 p2{
                    ((static_cast<float>(x2) - x_min) / (x_max - x_min)) - 0.5f,
                    ((static_cast<float>(y2) - y_min) / (y_max - y_min)) - 0.5f,
                    0.0
                };
                aa::vec3 p3{
                    ((static_cast<float>(x3) - x_min) / (x_max - x_min)) - 0.5f,
                    ((static_cast<float>(y3) - y_min) / (y_max - y_min)) - 0.5f,
                    0.0
                };
                float interp{100};
                for (float t = 0.0;
                     t < 1.0 + std::numeric_limits<float>::epsilon();
                     t += (1.0 / interp)) {
                    vertex_positions.push_back(
                        (1.0f - t) * ((1.0f - t) * p1 + t * p2) +
                        t * ((1.0f - t) * p2 + t * p3)
                    );
                }
            }
            ngi::gl::StaticBuffer test_b(vertex_positions, GL_ARRAY_BUFFER);
            ngi::gl::VertexArrayObject test_vao{};
            test_vao.attach_shader(test_s);
            test_vao.attach_buffer_object(test_b, 0, 3, GL_FLOAT, GL_FALSE, 0);
            vaos.push_back(std::move(test_vao));
            buffers.push_back(std::move(test_b));
            if (i % 2) {
                colors.push_back({1.0, 0.0, 0.0, 1.0});
            } else {
                colors.push_back({0.0, 0.0, 1.0, 1.0});
            }
        }

        while (!window.should_close()) {
            for (auto i = 0; i < vaos.size(); i++) {
                vaos[i].bind();
                test_s.update_uniform_vec4f("color", colors[i]);
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
