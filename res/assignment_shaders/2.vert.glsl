#version 450 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec2 vUv;

out vec2 uv;

uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;

void
main() {
    gl_Position = proj * view * model * vPosition;
    uv = vUv;
}
