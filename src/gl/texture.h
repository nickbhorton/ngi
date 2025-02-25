#pragma once

#include <glad/gl.h>
#include <string>

#include "arrayalgebra.h"

#ifdef NGI_LOG
#include "log/log.h"
extern Log glog;
#endif // DEBUG

namespace ngi::gl
{

class Texture
{
    GLuint name;
    int number_of_channels;

public:
    aa::ivec2 size;

    Texture(std::string const& path, int active_texture);
    ~Texture();
};

} // namespace ngi::gl
