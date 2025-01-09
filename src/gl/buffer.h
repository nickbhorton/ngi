#pragma once

#include <glad/gl.h>
#include <vector>

#ifdef NGI_LOG
#include "log/log.h"
extern Log glog;
#endif // DEBUG

namespace ngi::gl
{

template <typename T> class StaticBuffer
{
    GLuint name;
    GLenum bind_target;

    bool moved;

    GLsizei data_size;

public:
    ~StaticBuffer();
    StaticBuffer();

    StaticBuffer(T buffer_data, GLenum bind_target);
    StaticBuffer(std::vector<T> buffer_data, GLenum bind_target);

    // copying is disallowed
    StaticBuffer(StaticBuffer<T> const& other) = delete;
    StaticBuffer<T>& operator=(StaticBuffer<T> const& other) = delete;

    // moving is prefered
    StaticBuffer(StaticBuffer<T>&& other) noexcept;
    StaticBuffer<T>& operator=(StaticBuffer<T>&& other) noexcept;

    void bind();
    GLsizei byte_count();
    GLenum get_bind_target();
};

// default constructor
template <typename T>
StaticBuffer<T>::StaticBuffer() : name{}, bind_target{}, moved{true}
{
}

// generic constructor
template <typename T>
StaticBuffer<T>::StaticBuffer(T buffer_data, GLenum bind_target)
    : moved(false), bind_target(bind_target), data_size(sizeof(T))
{
    glCreateBuffers(1, &this->name);
    glNamedBufferStorage(name, this->data_size, &buffer_data, 0);
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::gl::buffer::Constructor",
        std::to_string(this->name) + std::string(" was constructed with ") +
            std::to_string(data_size) + std::string(" bytes")
    );
#endif
}

// generic vector constructor
template <typename T>
StaticBuffer<T>::StaticBuffer(std::vector<T> buffer_data, GLenum bind_target)
    : moved(false), bind_target(bind_target),
      data_size(sizeof(T) * buffer_data.size())
{
    glCreateBuffers(1, &name);
    glNamedBufferStorage(name, this->data_size, buffer_data.data(), 0);
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::gl::buffer::Constructor",
        std::to_string(this->name) + std::string(" was constructed with ") +
            std::to_string(sizeof(T)) + std::string("bytes ") +
            std::to_string(buffer_data.size()) + std::string(" times")
    );
#endif
}

// move constructor
template <typename T>
StaticBuffer<T>::StaticBuffer(StaticBuffer<T>&& other) noexcept
    : moved(false), bind_target(other.bind_target), data_size(other.data_size),
      name(other.name)
{
    other.moved = true;
}

// move assignment
template <typename T>
StaticBuffer<T>& StaticBuffer<T>::operator=(StaticBuffer<T>&& other) noexcept
{
    this->moved = false;
    this->bind_target = other.bind_target;
    this->name = other.name;
    this->data_size = other.data_size;
    other.moved = true;
    return *this;
}

template <typename T> void StaticBuffer<T>::bind()
{
    glBindBuffer(bind_target, name);
}

template <typename T> GLenum StaticBuffer<T>::get_bind_target()
{
    return bind_target;
}

template <typename T> GLsizei StaticBuffer<T>::byte_count()
{
    return data_size;
}

template <typename T> StaticBuffer<T>::~StaticBuffer()
{
    if (!moved) {
        glDeleteBuffers(1, &name);
#ifdef NGI_LOG
        glog.add(
            LogLevel::Status,
            "ngi::gl::buffer::Destructor",
            std::to_string(this->name) + std::string(" was destructed")
        );
#endif
    }
}

} // namespace ngi::gl
