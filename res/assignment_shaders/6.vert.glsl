#version 450 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vUv;

out vec2 uv;

void main() {
    gl_Position = vPosition;
    uv = vUv.xy;
}
