#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

void main() {
    fColor = vec4(uv.x,uv.y,0.0,1.0);
}
