#include "texture.h"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "aux/png/stb_image.h"

#include <iostream>

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
#ifdef NGI_LOG
        glog.add(
            LogLevel::Error,
            "ngi::gl::texture::Constructor",
            "texture at " + path + " failed to load data"
        );
#endif
        return;
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

#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::gl::texture::Constructor",
        "texture " + path + "(" + std::to_string(name) + ")" + " of size " +
            std::to_string(size[0]) + "x" + std::to_string(size[1]) + " with " +
            std::to_string(number_of_channels) + " channels was constructed"
    );
#endif
    stbi_image_free(data);
}

Texture::~Texture()
{
#ifdef NGI_LOG
    glog.add(
        LogLevel::Status,
        "ngi::gl::texture::Constructor",
        "texture " + std::to_string(name) + " was destructed"
    );
#endif
    glDeleteTextures(1, &name);
}
