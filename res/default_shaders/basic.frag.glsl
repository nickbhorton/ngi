#version 450 core

layout (location = 0) out vec4 fColor;

uniform vec4 color;

void main() {
    fColor = vec4(
        gl_FragCoord.x/800,
        gl_FragCoord.y/800,
        gl_FragCoord.z,
        1
    );
}
