// This is shader golf. Write the shortest shader possible

#include "arrayalgebra.h"
#include "aux/camera.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/texture.h"
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
bool cursor_control{true};

float camera_speed{0.05};

Camera first_person_camera(
    {0.5, 0.5, 1.25},
    90,
    0,
    {0, 1, 0},
    static_cast<float>(GraphicsWindowWidth) /
        static_cast<float>(GraphicsWindowHeight),
    0.1,
    1000,
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
    try {
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
        int current_s{0};
        std::vector<ngi::gl::ShaderProgram> shaders{};
        shaders.push_back(ngi::gl::ShaderProgram(
            {{"../res/assignment_shaders/7.vert.glsl", GL_VERTEX_SHADER},
             {"../res/assignment_shaders/7_diff.frag.glsl", GL_FRAGMENT_SHADER}}
        ));
        shaders.push_back(ngi::gl::ShaderProgram(
            {{"../res/assignment_shaders/7.vert.glsl", GL_VERTEX_SHADER},
             {"../res/assignment_shaders/7_norm.frag.glsl", GL_FRAGMENT_SHADER}}
        ));
        shaders.push_back(ngi::gl::ShaderProgram(
            {{"../res/assignment_shaders/7.vert.glsl", GL_VERTEX_SHADER},
             {"../res/assignment_shaders/7_disp.frag.glsl", GL_FRAGMENT_SHADER}}
        ));
        shaders.push_back(ngi::gl::ShaderProgram(
            {{"../res/assignment_shaders/7.vert.glsl", GL_VERTEX_SHADER},
             {"../res/assignment_shaders/7_lighting.frag.glsl",
              GL_FRAGMENT_SHADER}}
        ));
        shaders.push_back(ngi::gl::ShaderProgram(
            {{"../res/assignment_shaders/7_parallax.vert.glsl", GL_VERTEX_SHADER
             },
             {"../res/assignment_shaders/7_parallax.frag.glsl",
              GL_FRAGMENT_SHADER}}
        ));
        shaders.push_back(ngi::gl::ShaderProgram(
            {{"../res/assignment_shaders/7.vert.glsl", GL_VERTEX_SHADER},
             {"../res/assignment_shaders/7_diff_cat.frag.glsl",
              GL_FRAGMENT_SHADER}}
        ));
        shaders.push_back(ngi::gl::ShaderProgram(
            {{"../res/assignment_shaders/7_parallax.vert.glsl", GL_VERTEX_SHADER
             },
             {"../res/assignment_shaders/7_parallax_cat.frag.glsl",
              GL_FRAGMENT_SHADER}}
        ));
        int diff_s{0};
        int norm_s{1};
        int disp_s{2};
        int ligh_s{3};
        int para_s{4};
        int dcat_s{5};
        int pcat_s{6};

        // setup geometry
        std::array<aa::vec3, 6> plane_positions{
            {{0, 0, 0}, {0, 1, 0}, {1, 1, 0}, {1, 1, 0}, {1, 0, 0}, {0, 0, 0}}
        };
        ngi::gl::StaticBuffer<std::array<aa::vec3, 6>> plane_positions_b{
            plane_positions,
            GL_ARRAY_BUFFER
        };

        std::array<aa::vec2, 6> plane_uvs{
            {{0, 0}, {0, 1}, {1, 1}, {1, 1}, {1, 0}, {0, 0}}
        };
        ngi::gl::StaticBuffer<std::array<aa::vec2, 6>> plane_uvs_b{
            plane_uvs,
            GL_ARRAY_BUFFER
        };

        std::array<aa::vec3, 6> plane_normals{
            {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}
        };
        ngi::gl::StaticBuffer<std::array<aa::vec3, 6>> plane_normals_b{
            plane_normals,
            GL_ARRAY_BUFFER
        };

        std::array<aa::vec3, 6> plane_tangents{
            {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}}
        };
        ngi::gl::StaticBuffer<std::array<aa::vec3, 6>> plane_tangents_b{
            plane_tangents,
            GL_ARRAY_BUFFER
        };

        // setup vao
        ngi::gl::VertexArrayObject plane_vao{};

        plane_vao.attach_buffer_object(
            plane_positions_b,
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            0
        );
        plane_vao
            .attach_buffer_object(plane_uvs_b, 1, 2, GL_FLOAT, GL_FALSE, 0);
        plane_vao
            .attach_buffer_object(plane_normals_b, 2, 3, GL_FLOAT, GL_FALSE, 0);
        plane_vao.attach_buffer_object(
            plane_tangents_b,
            3,
            3,
            GL_FLOAT,
            GL_FALSE,
            0
        );

        // For my graphics to only render on left
        glViewport(0, 0, GraphicsWindowWidth, GraphicsWindowHeight);

        ngi::gl::Texture rock_diff{
            "../res/images/rock7/rock7_diff.jpg",
            GL_TEXTURE0
        };
        ngi::gl::Texture rock_disp{
            "../res/images/rock7/rock7_disp.png",
            GL_TEXTURE1
        };
        ngi::gl::Texture rock_norm{
            "../res/images/rock7/rock7_nor.png",
            GL_TEXTURE2
        };

        for (auto& s : shaders) {
            s.update_uniform_1i("diff", 0);
            s.update_uniform_1i("disp", 1);
            s.update_uniform_1i("norm", 2);
        }

        // uniforms
        float metallic{0.5};
        float metallic2{0.0};
        float roughness{0.5};
        float roughness2{0.6};
        float ao{0.5};
        float ao2{0.0};
        float thresh{0.625};
        float height_scale{0.1};
        float parallax_layers{128};
        aa::vec3 light_pos{0.5, 0.5, 1.0};
        aa::vec3 light_col{1, 1, 1};

        while (!window.should_close()) {
            key_frame_updates(window.get_window_ptr());

            // uniform
            shaders[current_s].bind();
            shaders[current_s].update_uniform_mat4f(
                "proj",
                first_person_camera.get_proj_matrix()
            );
            shaders[current_s].update_uniform_mat4f(
                "view",
                first_person_camera.get_view_matrix()
            );
            shaders[current_s].update_uniform_vec3f(
                "camera_position",
                first_person_camera.get_camera_position()
            );
            shaders[current_s].update_uniform_1f("metallic", metallic);
            shaders[current_s].update_uniform_1f("roughness", roughness);
            shaders[current_s].update_uniform_1f("ao", ao);
            shaders[current_s].update_uniform_1f("metallic2", metallic2);
            shaders[current_s].update_uniform_1f("roughness2", roughness2);
            shaders[current_s].update_uniform_1f("ao2", ao2);
            shaders[current_s].update_uniform_1f("thresh", thresh);
            shaders[current_s].update_uniform_1f("height_scale", height_scale);
            shaders[current_s].update_uniform_1f(
                "parallax_layers",
                parallax_layers
            );
            shaders[current_s].update_uniform_vec3f(
                "light_positions",
                light_pos
            );
            shaders[current_s].update_uniform_vec3f("light_colors", light_col);

            // background
            std::array<GLfloat, 4> static constexpr bg_color{0, 0, 0, 1};
            glClearBufferfv(GL_COLOR, 0, bg_color.data());
            glClear(GL_DEPTH_BUFFER_BIT);

            // Drawing to the canvas
            plane_vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, plane_positions.size());

            // Drawing to settings
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Assignment7", &open_imgui, window_flags_imgui);
            ImGui::SetWindowPos(ImVec2(GraphicsWindowWidth, 0));
            ImGui::SetWindowSize(
                ImVec2(SettingsWindowWidth, SettingsWindowHeight)
            );
            ImGui::RadioButton("Diffuse", &current_s, diff_s);
            ImGui::RadioButton("Diffuse Catagorize", &current_s, dcat_s);
            ImGui::RadioButton("Normal", &current_s, norm_s);
            ImGui::RadioButton("Displacement", &current_s, disp_s);
            ImGui::RadioButton("Flat Lighting", &current_s, ligh_s);
            ImGui::RadioButton("Parallax", &current_s, para_s);
            ImGui::RadioButton("Parallax Catagorize", &current_s, pcat_s);
            ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
            ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Ambient Occlusin", &ao, 0.0f, 1.0f);
            ImGui::SliderFloat("Grass Metallic", &metallic2, 0.0f, 1.0f);
            ImGui::SliderFloat("Grass Roughness", &roughness2, 0.0f, 1.0f);
            ImGui::SliderFloat("Grass Ambient Occlusin", &ao2, 0.0f, 1.0f);
            ImGui::SliderFloat("Height Scale", &height_scale, 0.0f, 1.0f);
            ImGui::SliderFloat("Parallax Layers", &parallax_layers, 0, 1024);
            ImGui::SliderFloat("Threshld", &thresh, 0, 2);
            ImGui::SliderFloat3("Light Position", light_pos.data(), 0, 2.0);
            ImGui::ColorEdit3("Light Color", light_col.data());
            ImGui::Text("current_s: %i, %.2f FPS", current_s, io.Framerate);
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            window.swap();
        }
    } catch (int& e) {
        std::cout << e << "\n";
    }
}
