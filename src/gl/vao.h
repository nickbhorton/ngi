#pragma once

#include "gl/buffer.h"
#include "gl/shader.h"

#include <glad/gl.h>

#define BUFFER_OFFSET(offset) ((void*)(offset))

namespace ngi::gl
{
class VertexArrayObject
{
    GLuint name;
    GLuint shader_name;

public:
    VertexArrayObject();
    ~VertexArrayObject();

    VertexArrayObject(VertexArrayObject const& other) = delete;
    VertexArrayObject& operator=(VertexArrayObject const& other) = delete;
    VertexArrayObject(VertexArrayObject&& other) noexcept = delete;
    VertexArrayObject& operator=(VertexArrayObject&& other) noexcept = delete;

    template <typename T>
    void attach_buffer_object(
        StaticBuffer<T>& buffer,
        GLuint index,
        GLint dimentions,
        GLenum type,
        GLboolean normalized,
        GLsizei stride
    );

    void attach_shader(ShaderProgram const& shader);

    template <typename T> void attach_element_array(StaticBuffer<T>& buffer);

    void bind();
};

template <typename T>
void VertexArrayObject::attach_buffer_object(
    StaticBuffer<T>& buffer,
    GLuint index,
    GLint dimentions,
    GLenum type,
    GLboolean normalized,
    GLsizei stride
)
{
    glBindVertexArray(name);
    buffer.bind();
    glVertexAttribPointer(
        index,           // index
        dimentions,      // number of numbers
        type,            // type
        normalized,      // normalized
        stride,          // stride aka how tightly packed are the numbers
        BUFFER_OFFSET(0) // how far into the buffer is the fisrt num
    );
    glEnableVertexAttribArray(index);
}

template <typename T>
void VertexArrayObject::attach_element_array(StaticBuffer<T>& buffer)
{
    glBindVertexArray(name);
    buffer.bind();
}

} // namespace ngi::gl
