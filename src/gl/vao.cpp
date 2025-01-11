#include "vao.h"

#ifdef NGI_LOG
#include "log/log.h"
extern Log glog;
#endif // NGI_LOG

using namespace ngi::gl;

VertexArrayObject::VertexArrayObject() : shader_name(0)
{
    glGenVertexArrays(1, &name);
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::gl::vao::Constructor",
        std::to_string(name) + std::string(" was constructed")
    );
#endif
}
void VertexArrayObject::attach_shader(ShaderProgram const& shader)
{
    shader_name = shader.get_name();
}

void VertexArrayObject::bind()
{
    glBindVertexArray(name);
    if (shader_name) {
        glUseProgram(shader_name);
    }
}

VertexArrayObject::~VertexArrayObject()
{
    if (!moved) {
#ifdef NGI_LOG
        glog.add(
            LogLevel::Status,
            "ngi::gl::vao::Destructor",
            std::to_string(name) + std::string(" was destructed")
        );
#endif
        glDeleteVertexArrays(1, &name);
    }
}

VertexArrayObject::VertexArrayObject(VertexArrayObject&& other) noexcept
    : moved(false), name(other.name), shader_name(other.shader_name)
{
    other.moved = true;
}

VertexArrayObject& VertexArrayObject::operator=(VertexArrayObject&& other
) noexcept
{
    moved = false;
    name = other.name;
    shader_name = other.shader_name;
    other.moved = true;
    return *this;
}
