#include "arrayalgebra.h"
#include "aux/camera.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/vao.h"
#include "log/log.h"

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
    {0, 0, 10},
    90.0f,
    0,
    {0, 1, 0},
    static_cast<float>(GraphicsWindowWidth) / static_cast<float>(GraphicsWindowHeight),
    0.1,
    100.0,
    45
);

aa::vec4 vary{0.25, 0.25, 0.75, 0.25};
aa::vec2 vary2{0.5, 0.75};
size_t shader_index = 0;
size_t constexpr shader_count = 5;

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

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && shader_index > 0) {
        shader_index--;
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS &&
        shader_index < shader_count - 1) {
        shader_index++;
        std::cout << shader_index << std::endl;
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

    float constexpr vary_amount = 0.01;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        vary[0] -= vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        vary[0] += vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        vary[1] -= vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        vary[1] += vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        vary[2] -= vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        vary[2] += vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        vary[3] -= vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        vary[3] += vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        vary2[0] -= vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        vary2[0] += vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        vary2[1] -= vary_amount;
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        vary2[1] += vary_amount;
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
                {"res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"res/assignment_shaders/default.2.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        auto circle_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"res/assignment_shaders/circle.2.frag.glsl", GL_FRAGMENT_SHADER
                }
            });
        auto rectangle_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"res/assignment_shaders/rectangle.2.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        auto qbezier_df_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"res/assignment_shaders/qbezier_df.2.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        auto qbezier_sdf_s =
            ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
                {"res/assignment_shaders/2.vert.glsl", GL_VERTEX_SHADER},
                {"res/assignment_shaders/qbezier_sdf.2.frag.glsl",
                 GL_FRAGMENT_SHADER}
            });
        std::array<ngi::gl::ShaderProgram, shader_count> shader_array{
            std::move(qbezier_sdf_s),
            std::move(qbezier_df_s),
            std::move(default_s),
            std::move(circle_s),
            std::move(rectangle_s)
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

        while (!window.should_close()) {
            std::array<GLfloat, 4> static constexpr bg_color{0, 0, 0.0, 1};
            glClearBufferfv(GL_COLOR, 0, bg_color.data());
            glClear(GL_DEPTH_BUFFER_BIT);

            cube_vao.set_shader(shader_array[shader_index]);
            cube_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, cube_pos.size());
            window.swap();

            key_frame_updates(window.get_window_ptr());
            shader_array[shader_index].update_uniform_mat4f(
                "proj",
                first_person_camera.get_proj_matrix()
            );
            shader_array[shader_index].update_uniform_mat4f(
                "view",
                first_person_camera.get_view_matrix()
            );
            shader_array[shader_index].update_uniform_vec4f("vary", vary);
            shader_array[shader_index].update_uniform_vec2f("vary2", vary2);

            shader_array[shader_index].update_uniform_mat4f(
                "model",
                aa::identity<float, 4>()
            );
            if (framebuffer_size_callback_active) {
                framebuffer_size_callback_active = false;
            }
        }
    } catch (int& e) {
        std::cout << "integer error thrown: " << e << "\n";
    }
}
