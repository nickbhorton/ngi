#version 450 core

layout (location = 0) out vec4 fColor;

uniform int window_width;
uniform int window_height;

void main() {
    fColor = vec4(
        gl_FragCoord.x/window_width,
        gl_FragCoord.y/window_height,
        gl_FragCoord.w,
        1.0
    );
}
