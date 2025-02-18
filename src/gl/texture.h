#pragma once

#include <glad/gl.h>
#include <string>

#include "arrayalgebra.h"

namespace ngi::gl
{

class Texture
{
    GLuint name;
    int number_of_channels;

public:
    aa::ivec2 size;

    Texture(std::string const& path);
    ~Texture();
};

} // namespace ngi::gl
