#include "glfw_wrapper.h"

#include <cstdlib>

#include "log/log.h"

extern Log glog;

using namespace ngi::glfw;

#ifdef NGI_LOG
void GLAPIENTRY gl_error_callback(
    GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
)
{
    std::string source_s{};
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        source_s = "API";
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        source_s = "APPLICATION";
        break;
    case GL_DEBUG_SOURCE_OTHER:
        source_s = "OTHER";
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        source_s = "SHADER_COMPILER";
        break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:
        source_s = "THIRD_PARTY";
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        source_s = "WINDOW_SYSTEM";
        break;
    }

    std::string severity_s{};
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        severity_s = "HIGH";
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        severity_s = "MEDIUM";
        break;
    case GL_DEBUG_SEVERITY_LOW:
        severity_s = "LOW";
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        severity_s = "NOTIFICATION";
        break;
    }

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            glog.add(
                LogLevel::CriticalError,
                "ngi::gl::error",
                std::to_string(id) + std::string(" severity: ") + severity_s +
                    std::string(" source: ") + source_s +
                    std::string(" message: ") +
                    std::string(message).substr(0, length - 1)
            );
            break;
        default:
            glog.add(
                LogLevel::Error,
                "ngi::gl::error_callback",
                std::to_string(id) + std::string(" severity: ") + severity_s +
                    std::string(" source: ") + source_s +
                    std::string(" message: ") +
                    std::string(message).substr(0, length - 1)
            );
        }
        throw 2;
        break;

    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        glog.add(
            LogLevel::Warning,
            "ngi::gl::undefined_behavior",
            std::to_string(id) + std::string(" severity: ") + severity_s +
                std::string(" source: ") + source_s +
                std::string(" message: ") +
                std::string(message).substr(0, length - 1)
        );
        break;

    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        glog.add(
            LogLevel::Warning,
            "ngi::gl::deprecated_behavior",
            std::to_string(id) + std::string(" severity: ") + severity_s +
                std::string(" source: ") + source_s +
                std::string(" message: ") +
                std::string(message).substr(0, length - 1)
        );
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        glog.add(
            LogLevel::Warning,
            "ngi::gl::performance",
            std::to_string(id) + std::string(" severity: ") + severity_s +
                std::string(" source: ") + source_s +
                std::string(" message: ") +
                std::string(message).substr(0, length - 1)
        );
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        glog.add(
            LogLevel::Warning,
            "ngi::gl::portability",
            std::to_string(id) + std::string(" severity: ") + severity_s +
                std::string(" source: ") + source_s +
                std::string(" message: ") +
                std::string(message).substr(0, length - 1)
        );
        break;
    case GL_DEBUG_TYPE_MARKER:
    case GL_DEBUG_TYPE_OTHER:
    case GL_DEBUG_TYPE_POP_GROUP:
    case GL_DEBUG_TYPE_PUSH_GROUP:
        glog.add(
            LogLevel::Warning,
            "ngi::gl::other_message",
            std::to_string(id) + std::string(" severity: ") + severity_s +
                std::string(" source: ") + source_s +
                std::string(" message: ") +
                std::string(message).substr(0, length - 1)
        );
    }
}
#endif

void glfw_error_callback(int error, const char* desc)
{
#ifdef NGI_LOG
    glog.add(
        LogLevel::CriticalError,
        "ngi::glfw::error_callback",
        std::to_string(error) + std::string(" ") + std::string(desc)
    );
#endif
    throw 2;
}

Wrapper::Wrapper()
{
    glfwInit();
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::Constructor",
        "glfwInit() was successfull"
    );
#endif
    glfwSetErrorCallback(glfw_error_callback);
}
Wrapper::~Wrapper()
{
    glfwTerminate();
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::Destructor",
        "glfwTerminate() was successfull"
    );
#endif
}

Window Wrapper::generate_window(int width, int height, GLFWkeyfun callback)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window w(
        glfwCreateWindow(width, height, "ngi window", NULL, NULL),
        width,
        height
    );
    if (!w.get_window_ptr()) {
        glfw_error_callback(-1, "window was null");
    }
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::generate_window()",
        "glfwCreateWindow() was succsessful"
    );
#endif
    glfwMakeContextCurrent(w.get_window_ptr());

    int version = gladLoadGL(glfwGetProcAddress);

    if (version == 0) {
#ifdef NGI_LOG
        glog.add(
            LogLevel::CriticalError,
            "ngi::glfw::Wrapper::generate_window()",
            "gladLoadGL() returned 0"
        );
#endif
        throw 2;
    }
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::generate_window()",
        "glad version " + std::to_string(GLAD_VERSION_MAJOR(version)) + "." +
            std::to_string(GLAD_VERSION_MINOR(version))
    );
#endif

    glfwSetKeyCallback(w.get_window_ptr(), callback);
    glViewport(0, 0, width, height);
#ifdef NGI_LOG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(gl_error_callback, 0);
#endif
    return w;
}

Window::Window(GLFWwindow* window, int width, int height)
    : window(window), width(width), height(height), destroy_responsibility(true)
{
}
Window::Window(Window&& other) noexcept
    : window(other.window), width(other.width), height(other.height),
      destroy_responsibility(true)
{
    other.destroy_responsibility = false;
}

Window& Window::operator=(Window&& other) noexcept
{
    this->window = other.window;
    this->width = other.width;
    this->height = other.height;
    this->destroy_responsibility = true;
    other.destroy_responsibility = false;
    return *this;
}

Window::~Window()
{
    if (destroy_responsibility) {
        glfwDestroyWindow(this->window);
#ifdef NGI_LOG
        glog.add(
            LogLevel::Status,
            "ngi::glfw::Wrapper::Destructor",
            "glfwDestroyWindow()"
        );
#endif
    } else {
#ifdef NGI_LOG
        glog.add(
            LogLevel::Status,
            "ngi::glfw::Wrapper::Destructor",
            "window without destroy responsibility was destructed"
        );
#endif
    }
}

bool Window::should_close() { return glfwWindowShouldClose(this->window); }
GLFWwindow* Window::get_window_ptr() { return this->window; }
int Window::get_width() { return this->width; }
int Window::get_height() { return this->height; }
void Window::swap()
{
    glfwPollEvents();
    glfwSwapBuffers(this->window);
}
