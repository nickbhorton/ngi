#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform sampler2D diff;
uniform sampler2D norm;
uniform sampler2D disp;

void main() {
    vec3 d = texture(diff, uv).rgb;
    float val = d.g - d.b;
    fColor = vec4(vec3(step(0.2, val)), 1.0);
}
