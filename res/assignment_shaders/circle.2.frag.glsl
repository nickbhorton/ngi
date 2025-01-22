#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform vec4 vary;

void main() {
    vec2 org = vec2(vary.x, vary.y);
    vec2 pos = 2.0 * (uv - 0.5);

    float radius = vary.z;

    float red = distance(pos, org) - radius;
    float blue = 0.0;
    if (red < 0.0) {
        blue = -red;
    }
    fColor = vec4(
        red,
        1.0 - step(0.01, fract(red)),
        blue,
        1.0
    );
}
