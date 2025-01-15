// This is shader golf. Write the shortest shader possible

#include "arrayalgebra.h"
#include "aux/camera.h"
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

int WindowWidth{1920};
int WindowHeight{1080};

int last_x{WindowWidth / 2};
int last_y{WindowHeight / 2};

float mouse_xoffset{0.0f};
float mouse_yoffset{0.0f};

bool update_mouse{false};

Camera first_person_camera(
    {0, 0, 10},
    90.0f,
    0,
    {0, 1, 0},
    static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight),
    0.1,
    1000.0,
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
    WindowWidth = width;
    WindowHeight = height;
    framebuffer_size_callback_active = true;
    first_person_camera.set_aspect_ratio(
        static_cast<GLfloat>(WindowWidth) / static_cast<GLfloat>(WindowHeight)
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
    update_mouse = true;
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

    if (update_mouse) {
        first_person_camera.get_pitch_ref() -= mouse_yoffset;
        first_person_camera.get_yaw_ref() += mouse_xoffset;

        if (first_person_camera.get_pitch() > 89.0f) {
            first_person_camera.get_pitch_ref() = 89.0f;
        }
        if (first_person_camera.get_pitch() < -89.0f) {
            first_person_camera.get_pitch_ref() = -89.0f;
        }
        update_mouse = false;
    }
}

int main(int argc, char** argv)
{
    // try catch abstraction because throwing ensures destruction of all
    // objects.
    try {
        ngi::glfw::Wrapper wrap{};
        ngi::glfw::Window window{
            wrap.generate_window(WindowWidth, WindowHeight, key_callback)
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

        ngi::gl::ShaderProgram test_s(
            {{"../res/assignment_shaders/assignment1.vert.glsl", GL_VERTEX_SHADER},
             {"../res/assignment_shaders/assignment1.frag.glsl", GL_FRAGMENT_SHADER}}
        );
        test_s.update_uniform_1i("window_width", WindowWidth);
        test_s.update_uniform_1i("window_height", WindowHeight);

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
            std::array<GLfloat, 4> static constexpr bg_color{0, 0, 0, 1};
            glClearBufferfv(GL_COLOR, 0, bg_color.data());
            glClear(GL_DEPTH_BUFFER_BIT);

            cube_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, cube.size());
            window.swap();

            key_frame_updates(window.get_window_ptr());
            test_s.update_uniform_mat4f(
                "proj",
                first_person_camera.get_proj_matrix()
            );
            test_s.update_uniform_mat4f(
                "view",
                first_person_camera.get_view_matrix()
            );
            if (framebuffer_size_callback_active) {
                test_s.update_uniform_1i("window_width", WindowWidth);
                test_s.update_uniform_1i("window_height", WindowHeight);
                framebuffer_size_callback_active = false;
            }
        }
    } catch (int& e) {
        std::cout << "integer error thrown: " << e << "\n";
    }
}
