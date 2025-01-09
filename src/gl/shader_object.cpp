#include "shader.h"

#include <fstream>

#ifdef NGI_LOG
#include "log/log.h"
extern Log glog;
#endif // DEBUG

using namespace ngi::gl;

void report_shader_info_log(GLuint name, std::string const& msg)
{
    GLsizei info_log_length{};
    glGetShaderiv(name, GL_INFO_LOG_LENGTH, &info_log_length);
    std::string error_log("\0", info_log_length);
    glGetShaderInfoLog(
        name,
        info_log_length,
        &info_log_length,
        (GLchar*)error_log.data()
    );
#ifdef NGI_LOG
    glog.add(
        LogLevel::CriticalError,
        "ngi::gl::shader_object::Constructor",
        msg + std::string(": ") + error_log
    );
#endif
}

ShaderObject::ShaderObject(
    std::string const& shader_filename,
    GLenum shader_type
)
    : moved(false)
{
    name = glCreateShader(shader_type);
    if (!name) {
#ifdef NGI_LOG
        glog.add(
            LogLevel::CriticalError,
            "ngi::gl::shader_object::Constructor",
            shader_filename + std::string(" glCreateShader() return 0")
        );
#endif
        throw 1;
    }
    std::fstream file{};
    file.open(shader_filename);
    if (file.bad() || file.fail() || !file.good()) {
#ifdef NGI_LOG
        glog.add(
            LogLevel::CriticalError,
            "ngi::gl::shader_object::Constructor",
            shader_filename + std::string(" could not find file")
        );
#endif
        throw 1;
    }
    std::string shader_source{};
    char file_byte{};
    while (!file.eof()) {
        file_byte = file.get();
        if (file.eof()) {
            break;
        }
        shader_source.push_back(file_byte);
    }
    const char* shader_source_ptr{shader_source.data()};
    const GLint shader_source_length{static_cast<GLint>(shader_source.size())};

    glShaderSource(name, 1, &shader_source_ptr, &shader_source_length);
    glCompileShader(name);

    GLint compile_status{};
    glGetShaderiv(name, GL_COMPILE_STATUS, &compile_status);
    if (compile_status != GL_TRUE) {
        report_shader_info_log(
            name,
            "Shader Compilation Error " + shader_filename
        );
        throw 1;
    }
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::gl::shader_object::Constructor",
        std::to_string(name) + std::string(" aka ") + shader_filename +
            std::string(" compiled successfully")
    );
#endif
}

auto ShaderObject::get_name() const -> GLuint { return name; }

ShaderObject::~ShaderObject()
{
    if (!moved) {
#ifdef NGI_LOG
        glog.add(
            LogLevel::Status,
            "ngi::gl::shader_object::Destructor",
            std::to_string(name) + std::string(" was destructed")
        );
#endif
        glDeleteShader(name);
    }
}

ShaderObject::ShaderObject(ShaderObject&& other) noexcept
    : name(other.name), moved(false)
{
    other.moved = true;
}
