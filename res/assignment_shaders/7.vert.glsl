#version 450 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vUvs;

out vec2 uv;
out vec3 frag_pos;

uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * vPosition;
    frag_pos = vec3(vPosition);
    uv = vUvs.xy;
}
