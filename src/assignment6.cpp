#include "arrayalgebra.h"
#include "aux/glfw_wrapper.h"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/texture.h"
#include "gl/vao.h"

#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
#include "imgui/imgui.h"

#include <GLFW/glfw3.h>

int WindowWidth{1200};
int WindowHeight{800};

int ImageWindowWidth{800};
int ImageWindowHeight{800};

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
    // glfwSwapInterval(0);

    glfwSetFramebufferSizeCallback(
        window.get_window_ptr(),
        framebuffer_size_callback
    );
    // glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.IniFilename = NULL;

    ImGui_ImplGlfw_InitForOpenGL(window.get_window_ptr(), true);
    ImGui_ImplOpenGL3_Init();

    std::vector<ngi::gl::ShaderProgram> shaders{};
    shaders.push_back(
        ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
            {"./res/assignment_shaders/6.vert.glsl", GL_VERTEX_SHADER},
            {"./res/assignment_shaders/dog.6.frag.glsl", GL_FRAGMENT_SHADER}
        })
    );
    shaders.push_back(
        ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
            {"./res/assignment_shaders/6.vert.glsl", GL_VERTEX_SHADER},
            {"./res/assignment_shaders/thresh_dog.6.frag.glsl",
             GL_FRAGMENT_SHADER}
        })
    );
    shaders.push_back(
        ngi::gl::ShaderProgram(std::vector<std::pair<std::string, GLenum>>{
            {"./res/assignment_shaders/6.vert.glsl", GL_VERTEX_SHADER},
            {"./res/assignment_shaders/tanh_dog.6.frag.glsl", GL_FRAGMENT_SHADER
            }
        })
    );

    int current_shader{0};
    ngi::gl::Texture tex1{"./res/images/zebras.jpg"};

    for (auto& s : shaders) {
        s.update_uniform_vec2f(
            "img_res",
            {static_cast<float>(tex1.size[0]), static_cast<float>(tex1.size[1])}
        );
    }

    std::array<aa::vec2, 4> quad_uv{{{0, 0}, {0, 1}, {1, 1}, {1, 0}}};
    float image_max_x{
        (2.0f * (static_cast<float>(ImageWindowWidth) /
                 static_cast<float>(WindowWidth))) -
        1.0f
    };
    std::array<aa::vec3, 4> quad_pos{
        {{-1, -1, 0}, {-1, 1, 0}, {image_max_x, 1, 0}, {image_max_x, -1, 0}}
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
    quad_vao.attach_buffer_object(quad_pos_b, 0, 3, GL_FLOAT, GL_FALSE, 0);
    quad_vao.attach_buffer_object(quad_uv_b, 1, 2, GL_FLOAT, GL_FALSE, 0);

    // diff of gaussian stuff
    float dog_k{1.6};
    float dog_sig{1.4};
    float dog_tau{1.0};
    float dog_threshhold{0.5};
    int dog_radius{5};
    float dog_phi{11.0};

    while (!window.should_close()) {
        glfwPollEvents();

        shaders[current_shader].bind();
        shaders[current_shader].update_uniform_1f("k", dog_k);
        shaders[current_shader].update_uniform_1f("sig", dog_sig);
        shaders[current_shader].update_uniform_1f(
            "radius",
            static_cast<float>(dog_radius)
        );
        shaders[current_shader].update_uniform_1f("tau", dog_tau);
        shaders[current_shader].update_uniform_1f("thresh", dog_threshhold);
        shaders[current_shader].update_uniform_1f("phi", dog_phi);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiWindowFlags window_flags{};
        window_flags |= ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoBackground;
        bool open{true};
        ImGui::Begin("Difference of Gaussians", &open, window_flags);
        ImGui::SetWindowPos(ImVec2(ImageWindowWidth, 0), ImGuiCond_Once);
        ImGui::SetWindowSize(
            ImVec2(WindowWidth - ImageWindowWidth, WindowHeight),
            ImGuiCond_Once
        );
        ImGui::RadioButton("dog", &current_shader, 0);
        ImGui::SameLine();
        ImGui::RadioButton("thesholded", &current_shader, 1);
        ImGui::SameLine();
        ImGui::RadioButton("tanh", &current_shader, 2);
        ImGui::SliderInt("radius", &dog_radius, 1, 10);
        ImGui::SliderFloat("sigma", &dog_sig, 0.0f, 10.0f);
        ImGui::SliderFloat("k", &dog_k, 0.0f, 2.0f);
        ImGui::SliderFloat("tau", &dog_tau, 0.0f, 2.0f);
        ImGui::SliderFloat("threshold", &dog_threshhold, 0.0f, 3.0f);
        ImGui::SliderFloat("tanh phi", &dog_phi, 0.0, 300.0);
        ImGui::Text("%.2f FPS", io.Framerate);
        ImGui::End();

        std::array<GLfloat, 4> static constexpr bg_color{0, 0, 0, 1};
        glClearBufferfv(GL_COLOR, 0, bg_color.data());
        glClear(GL_DEPTH_BUFFER_BIT);

        quad_vao.bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, quad_pos.size());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swap();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
