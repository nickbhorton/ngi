#include "glfw_wrapper.h"

#ifdef NGI_LOG
#include "log/log.h"
static Log log{};
#endif

#include <iostream>

using namespace ngi::glfw;

void error_callback(int error, const char* desc)
{
    std::cerr << "glfw Error(" << error << "): " << desc << "\n";
}

Wrapper::Wrapper()
{
    if (glfwInit() != GLFW_TRUE) {
#ifdef NGI_LOG
        log.add(
            LogLevel::CriticalError,
            "ngi::glfw::Wrapper::Constructor",
            "glfwInit() failed"
        );
#endif
        std::exit(1);
    }
#ifdef NGI_LOG
    log.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::Constructor",
        "glfwInit() was successfull"
    );
#endif
    glfwSetErrorCallback(error_callback);
}
Wrapper::~Wrapper()
{
    glfwTerminate();
#ifdef NGI_LOG
    log.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::Destructor",
        "glfwTerminate() was successfull"
    );
#endif
}

Window Wrapper::generate_window(int width, int height)
{
    Window w{};
    w.width = width;
    w.height = height;
    w.window = glfwCreateWindow(width, height, "ngi window", NULL, NULL);
    if (!w.window) {
#ifdef NGI_LOG
        log.add(
            LogLevel::CriticalError,
            "ngi::glfw::Wrapper::generate_window()",
            "glfwCreateWindow() failed"
        );
#endif
        std::exit(1);
    }
#ifdef NGI_LOG
    log.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::generate_window()",
        "glfwCreateWindow() was succsessful"
    );
#endif
    glfwMakeContextCurrent(w.window);
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
#ifdef NGI_LOG
        log.add(
            LogLevel::CriticalError,
            "ngi::glfw::Wrapper::generate_window()",
            "gladLoadGL() returned 0"
        );
#endif
        std::exit(1);
    }
#ifdef NGI_LOG
    log.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::generate_window()",
        "glad version " + std::to_string(GLAD_VERSION_MAJOR(version)) + "." +
            std::to_string(GLAD_VERSION_MINOR(version))
    );
#endif
    return w;
}

Window::~Window()
{
    glfwDestroyWindow(this->window);
#ifdef NGI_LOG
    log.add(
        LogLevel::Status,
        "ngi::glfw::Wrapper::Destructor",
        "glfwDestroyWindow()"
    );
#endif
}
