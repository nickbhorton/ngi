#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform sampler2D diff;
uniform sampler2D norm;
uniform sampler2D disp;

void main() {
    fColor = texture(disp, uv);
}
