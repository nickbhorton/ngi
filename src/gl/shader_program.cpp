#include "shader.h"

#include <iostream>

using namespace ngi::gl;

void report_program_info_log(GLuint name, std::string const& msg)
{
    GLsizei info_log_length{};
    glGetProgramiv(name, GL_INFO_LOG_LENGTH, &info_log_length);
    std::string error_log("\0", info_log_length);
    glGetProgramInfoLog(
        name,
        info_log_length,
        &info_log_length,
        error_log.data()
    );
    std::cerr << msg << ": " << error_log;
}

ShaderProgram::ShaderProgram(std::vector<std::pair<std::string, GLenum>> shaders
)
{
    name = glCreateProgram();
    if (!name) {
        std::cerr << "Shader linking error: glCreateProgram() returned zero\n";
        std::exit(EXIT_FAILURE);
    }
    for (auto const& [filename, shader_type] : shaders) {
        ShaderObject so(filename, shader_type);
        sos.push_back(std::move(so));
    }
    for (auto const& so : sos) {
        this->attatch(so);
    }
    this->link();
}

void ShaderProgram::attatch(ShaderObject const& shader)
{
    glAttachShader(name, shader.get_name());
}

void ShaderProgram::link()
{
    glLinkProgram(name);
    GLint link_status{};
    glGetProgramiv(name, GL_LINK_STATUS, &link_status);
    if (link_status != GL_TRUE) {
        report_program_info_log(name, "Shader Link Error");
        std::exit(EXIT_FAILURE);
    }
}

void ShaderProgram::bind() { glUseProgram(name); }

auto ShaderProgram::get_name() const -> GLuint { return name; }

ShaderProgram::~ShaderProgram() { glDeleteProgram(name); }

void ShaderProgram::update_uniform_1f(
    std::string const& uniform_name,
    GLfloat new_val
)
{
    this->bind();
    GLint location = glGetUniformLocation(name, uniform_name.c_str());
    glUniform1f(location, new_val);
}

void ShaderProgram::update_uniform_1i(
    std::string const& uniform_name,
    GLint new_val
)
{
    this->bind();
    GLint location = glGetUniformLocation(name, uniform_name.c_str());
    glUniform1i(location, new_val);
}

void ShaderProgram::update_uniform_mat4f(
    std::string const& uniform_name,
    aa::mat4 const& new_val
)
{
    this->bind();
    GLint location = glGetUniformLocation(name, uniform_name.c_str());
    auto flat_data = aa::flatten(new_val);
    glUniformMatrix4fv(location, 1, GL_FALSE, flat_data.data());
}

void ShaderProgram::update_uniform_vec3f(
    std::string const& uniform_name,
    aa::vec3 const& new_val
)
{
    this->bind();
    GLint location = glGetUniformLocation(name, uniform_name.c_str());
    glUniform3fv(location, 1, new_val.data());
}
