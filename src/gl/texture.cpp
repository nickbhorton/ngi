#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "aux/png/stb_image.h"

#include <iostream>
#include <stdexcept>

using namespace ngi::gl;

// adapted from learnopengl
Texture::Texture(std::string const& path)
{

    glGenTextures(1, &name);
    glBindTexture(GL_TEXTURE_2D, name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    unsigned char* data =
        stbi_load(path.c_str(), &size[0], &size[1], &number_of_channels, 0);

    if (!data) {
        throw std::runtime_error("Failed to load texture from " + path);
    }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        size[0],
        size[1],
        0,
        number_of_channels == 4 ? GL_RGBA : GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    std::cout << path << ": (" << size[0] << ", " << size[1] << ") "
              << number_of_channels << "\n";

    stbi_image_free(data);
}

Texture::~Texture() { glDeleteTextures(1, &name); }
