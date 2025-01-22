#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform vec4 vary;

void main() {
    vec2 p = 2.0 * (uv - 0.5);
    // [0, ~0.75]
    vec2 o = vec2(vary.x, vary.y);
    float r = fract(vary.z);
    float c = step(1.0 - r, 1.0 - distance(p, o));
    fColor = vec4(
        vec3(c,c,c),
        1.0
    );
}
