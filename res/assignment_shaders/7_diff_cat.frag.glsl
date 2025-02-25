#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform sampler2D diff;
uniform sampler2D norm;
uniform sampler2D disp;

uniform float thresh;

void main() {
    vec3 d = texture(diff, uv).rgb;
    float val = d.g / (d.b + d.r);
    fColor = vec4(vec3(step(thresh, val)), 1.0);
}
