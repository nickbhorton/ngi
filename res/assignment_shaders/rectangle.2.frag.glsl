#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform vec4 vary;

void main() {
    vec2 org = vec2(vary.x,vary.y);
    vec2 pos = 2.0 * (uv - 0.5);

    vec2 dist = vec2(
        distance(org.x, pos.x),
        distance(org.y, pos.y)
    );
    vec2 ab = 2.0 * vec2(vary.z,vary.w);

    float red = max(dist.x - ab.x * 0.5, dist.y - ab.y * 0.5);
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
