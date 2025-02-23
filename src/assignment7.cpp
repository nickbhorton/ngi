// This is shader golf. Write the shortest shader possible

#include "arrayalgebra.h"
#include "aux/camera.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/vao.h"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/imgui.h"

#ifdef NGI_LOG
#include "log/log.h"
Log glog{};
#endif

int WindowHeight{800};
int WindowWidth{1200};

int GraphicsWindowWidth{WindowWidth / 3 * 2};
int GraphicsWindowHeight{WindowHeight};

int SettingsWindowWidth{WindowWidth / 3};
int SettingsWindowHeight{WindowHeight};

int last_x{GraphicsWindowWidth / 2};
int last_y{GraphicsWindowHeight / 2};

float mouse_xoffset{0.0f};
float mouse_yoffset{0.0f};

bool cursor_update{false};
bool cursor_control{false};

Camera first_person_camera(
    {0.5, 0.5, 3},
    90.0f,
    0,
    {0, 1, 0},
    static_cast<float>(GraphicsWindowWidth) /
        static_cast<float>(GraphicsWindowHeight),
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
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        if (cursor_control) {
            cursor_control = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else {
            cursor_control = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

bool framebuffer_size_callback_active{false};
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WindowWidth = width;
    WindowHeight = height;

    GraphicsWindowWidth = WindowWidth / 3 * 2;
    GraphicsWindowHeight = WindowHeight;
    SettingsWindowWidth = WindowWidth / 3;
    SettingsWindowHeight = WindowHeight;

    framebuffer_size_callback_active = true;

    first_person_camera.set_aspect_ratio(
        static_cast<GLfloat>(GraphicsWindowWidth) /
        static_cast<GLfloat>(GraphicsWindowHeight)
    );

    glViewport(0, 0, GraphicsWindowWidth, GraphicsWindowHeight);
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

    if (cursor_update && !cursor_control) {
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
    // glfw and window setup
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

    // setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = NULL;

    ImGui_ImplGlfw_InitForOpenGL(window.get_window_ptr(), true);
    ImGui_ImplOpenGL3_Init();

    ImGuiWindowFlags window_flags_imgui{};
    window_flags_imgui |= ImGuiWindowFlags_NoMove;
    window_flags_imgui |= ImGuiWindowFlags_NoResize;
    window_flags_imgui |= ImGuiWindowFlags_NoTitleBar;
    window_flags_imgui |= ImGuiWindowFlags_NoCollapse;
    window_flags_imgui |= ImGuiWindowFlags_NoBackground;
    bool open_imgui{true};

    // setup shaders
    ngi::gl::ShaderProgram test_s(
        {{"../res/assignment_shaders/1.vert.glsl", GL_VERTEX_SHADER},
         {"../res/assignment_shaders/1.frag.glsl", GL_FRAGMENT_SHADER}}
    );
    test_s.update_uniform_1i("window_width", GraphicsWindowWidth);
    test_s.update_uniform_1i("window_height", GraphicsWindowHeight);

    // setup geometry
    std::array<aa::vec3, 6> plane{
        {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 1, 0}, {1, 0, 0}, {0, 0, 0}}
    };
    ngi::gl::StaticBuffer<std::array<aa::vec3, 6>> cube_b{
        plane,
        GL_ARRAY_BUFFER
    };

    // setup vao
    ngi::gl::VertexArrayObject plane_vao{};
    plane_vao.set_shader(test_s);
    plane_vao.attach_buffer_object(cube_b, 0, 3, GL_FLOAT, GL_FALSE, 0);

    // For my graphics to only render on left
    glViewport(0, 0, GraphicsWindowWidth, GraphicsWindowHeight);

    while (!window.should_close()) {
        std::array<GLfloat, 4> static constexpr bg_color{0, 0, 0, 1};
        glClearBufferfv(GL_COLOR, 0, bg_color.data());
        glClear(GL_DEPTH_BUFFER_BIT);

        // Drawing to the canvas
        plane_vao.bind();
        glDrawArrays(GL_TRIANGLES, 0, plane.size());

        // Drawing to settings
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Assignment7", &open_imgui, window_flags_imgui);
        ImGui::SetWindowPos(ImVec2(800, 0), ImGuiCond_Once);
        ImGui::SetWindowSize(
            ImVec2(SettingsWindowWidth, SettingsWindowHeight),
            ImGuiCond_Once
        );
        ImGui::Text("%.2f FPS", io.Framerate);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
            test_s.update_uniform_1i("window_width", GraphicsWindowWidth);
            test_s.update_uniform_1i("window_height", GraphicsWindowHeight);
            framebuffer_size_callback_active = false;
        }
    }
}
