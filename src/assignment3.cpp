#include "arrayalgebra.h"
#include "aux/camera.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/vao.h"
#include <GLFW/glfw3.h>

#include <chrono>

#ifdef NGI_LOG
#include "log/log.h"
Log glog{};
#endif

int GraphicsWindowWidth{1920};
int GraphicsWindowHeight{1080};

int last_x{GraphicsWindowWidth / 2};
int last_y{GraphicsWindowHeight / 2};

float mouse_xoffset{0.0f};
float mouse_yoffset{0.0f};

bool cursor_update{false};

Camera first_person_camera(
    {0, 0, 1.8},
    90.0f,
    0,
    {0, 1, 0},
    static_cast<float>(GraphicsWindowWidth) / static_cast<float>(GraphicsWindowHeight),
    0.1,
    100.0,
    45
);

static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_O && action == GLFW_PRESS) {
        first_person_camera.set_orthogonal_mode();
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        first_person_camera.set_perspective_mode();
    }
}

bool framebuffer_size_callback_active{false};
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    GraphicsWindowWidth = width;
    GraphicsWindowHeight = height;
    framebuffer_size_callback_active = true;
    first_person_camera.set_aspect_ratio(
        static_cast<GLfloat>(GraphicsWindowWidth) / static_cast<GLfloat>(GraphicsWindowHeight)
    );
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    mouse_xoffset = xpos - last_x;
    mouse_yoffset = last_y - ypos;
    last_x = xpos;
    last_y = ypos;

    float constexpr sensitivity = 0.1f;
    mouse_xoffset *= sensitivity;
    mouse_yoffset *= sensitivity;
    cursor_update = true;
}

void key_frame_updates(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        first_person_camera.get_camera_position_ref() -=
            camera_speed * first_person_camera.get_camera_front();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        first_person_camera.get_camera_position_ref() +=
            camera_speed * first_person_camera.get_camera_front();
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        first_person_camera.get_camera_position_ref() +=
            camera_speed * aa::normalize(aa::cross(
                               first_person_camera.get_camera_front(),
                               first_person_camera.get_camera_up()
                           ));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        first_person_camera.get_camera_position_ref() -=
            camera_speed * aa::normalize(aa::cross(
                               first_person_camera.get_camera_front(),
                               first_person_camera.get_camera_up()
                           ));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        first_person_camera.get_camera_position_ref() +=
            camera_speed * aa::vec3({0, 1, 0});
        ;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        first_person_camera.get_camera_position_ref() -=
            camera_speed * aa::vec3({0, 1, 0});
    }

    if (cursor_update) {
        first_person_camera.get_pitch_ref() -= mouse_yoffset;
        first_person_camera.get_yaw_ref() += mouse_xoffset;

        if (first_person_camera.get_pitch() > 89.0f) {
            first_person_camera.get_pitch_ref() = 89.0f;
        }
        if (first_person_camera.get_pitch() < -89.0f) {
            first_person_camera.get_pitch_ref() = -89.0f;
        }
        cursor_update = false;
    }
}

int main(int argc, char** argv)
{
    // try catch abstraction because throwing ensures destruction of all
    // objects.
    try {
        ngi::glfw::Wrapper wrap{};
        ngi::glfw::Window window{
            wrap.generate_window(GraphicsWindowWidth, GraphicsWindowHeight, key_callback)
        };
        // turn off vsync
        glfwSwapInterval(0);
        glfwSetFramebufferSizeCallback(
            window.get_window_ptr(),
            framebuffer_size_callback
        );
        glfwSetCursorPosCallback(window.get_window_ptr(), mouse_callback);
        glfwSetInputMode(
            window.get_window_ptr(),
            GLFW_CURSOR,
            GLFW_CURSOR_DISABLED
        );
        glEnable(GL_DEPTH_TEST);

        auto default_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"./res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"./res/assignment_shaders/default3.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        auto glyph1_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"./res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"./res/assignment_shaders/glyph1_3.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        auto glyph2_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"./res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"./res/assignment_shaders/glyph2_3.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        auto glyph3_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"./res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"./res/assignment_shaders/glyph3_3.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        auto glyph4_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"./res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"./res/assignment_shaders/glyph4_3.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        size_t constexpr shader_count{5};
        std::array<ngi::gl::ShaderProgram, shader_count> shader_array{
            std::move(default_s),
            std::move(glyph1_s),
            std::move(glyph2_s),
            std::move(glyph3_s),
            std::move(glyph4_s),
        };

        std::array<aa::vec3, 8> cube_gen_pos{{
            {0.5, -0.5, -0.5},
            {0.5, -0.5, 0.5},
            {-0.5, -0.5, 0.5},
            {-0.5, -0.5, -0.5},
            {0.5, 0.5, -0.5},
            {0.5, 0.5, 0.5},
            {-0.5, 0.5, 0.5},
            {-0.5, 0.5, -0.5},
        }};
        std::array<aa::vec2, 4> cube_gen_uvs{
            {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}}
        };
        std::array<aa::ivec2, 36> cube_gen_indexs{{
            {2, 1}, {3, 2}, {4, 3}, {5, 3}, {8, 4}, {7, 1}, {5, 3}, {6, 4},
            {2, 1}, {2, 2}, {6, 3}, {7, 4}, {7, 3}, {8, 4}, {4, 1}, {5, 4},
            {1, 1}, {4, 2}, {1, 4}, {2, 1}, {4, 3}, {6, 2}, {5, 3}, {7, 1},
            {1, 2}, {5, 3}, {2, 1}, {3, 1}, {2, 2}, {7, 4}, {3, 2}, {7, 3},
            {4, 1}, {8, 3}, {5, 4}, {4, 2},
        }};

        std::array<aa::vec3, 36> cube_pos{};
        std::array<aa::vec2, 36> cube_uv{};

        for (size_t i = 0; i < 36; i++) {
            auto const& [pi, uvi] = cube_gen_indexs[i];
            cube_pos[i] = cube_gen_pos[pi - 1];
            cube_uv[i] = cube_gen_uvs[uvi - 1];
        }

        ngi::gl::StaticBuffer<std::array<aa::vec3, 36>> cube_pos_b{
            cube_pos,
            GL_ARRAY_BUFFER
        };
        ngi::gl::StaticBuffer<std::array<aa::vec2, 36>> cube_uv_b{
            cube_uv,
            GL_ARRAY_BUFFER
        };
        ngi::gl::VertexArrayObject cube_vao{};
        cube_vao.set_shader(shader_array[0]);
        cube_vao.attach_buffer_object(cube_pos_b, 0, 3, GL_FLOAT, GL_FALSE, 0);
        cube_vao.attach_buffer_object(cube_uv_b, 1, 2, GL_FLOAT, GL_FALSE, 0);

        size_t constexpr frame_count = 600;
        for (size_t shader_index = 0; shader_index < shader_array.size();
             shader_index++) {
            shader_array[shader_index].update_uniform_mat4f(
                "model",
                aa::identity<float, 4>()
            );
            shader_array[shader_index].update_uniform_mat4f(
                "view",
                first_person_camera.get_view_matrix()
            );
            shader_array[shader_index].update_uniform_mat4f(
                "proj",
                first_person_camera.get_proj_matrix()
            );
            std::array<double, frame_count - 1> measurments{};
            for (size_t i = 0; i < frame_count; i++) {
                std::array<GLfloat, 4> static constexpr bg_color{0, 0, 0.0, 1};
                glClearBufferfv(GL_COLOR, 0, bg_color.data());
                glClear(GL_DEPTH_BUFFER_BIT);

                cube_vao.set_shader(shader_array[shader_index]);
                cube_vao.bind();

                auto start_time = std::chrono::high_resolution_clock::now();
                glDrawArrays(GL_TRIANGLES, 0, cube_pos.size());
                window.swap();
                auto end_time = std::chrono::high_resolution_clock::now();

                auto duration_us =
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        end_time - start_time
                    );
                // throw away frame 0 because it unusually fast
                if (i != 0) {
                    measurments[i - 1] =
                        static_cast<double>(duration_us.count());
                }

                key_frame_updates(window.get_window_ptr());
                if (framebuffer_size_callback_active) {
                    framebuffer_size_callback_active = false;
                }
            }
            double mean = 0.0;
            for (auto const& m : measurments) {
                mean += m;
            }
            mean /= static_cast<double>(measurments.size());
            double stddev = 0.0;
            for (auto const& m : measurments) {
                stddev += std::pow((m - mean), 2);
            }
            stddev /= static_cast<double>(measurments.size() - 1);
            stddev = std::sqrt(stddev);
            std::cout << "shader " << shader_index << ": ";
            std::cout << mean / 1000.0 << " ms +/- ";
            std::cout << stddev / 1000.0 << " ms\n";
        }
    } catch (int& e) {
        std::cout << "integer error thrown: " << e << "\n";
    }
}
