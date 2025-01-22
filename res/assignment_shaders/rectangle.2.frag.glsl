#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform vec4 vary;

void main() {
    vec2 p = 2.0 * (uv - 0.5);
    vec2 o = vec2(vary.x,vary.y);
    vec2 dist = vec2(
        distance(o.x, p.x),
        distance(o.y, p.y)
    );
    vec2 ab = 2.0 * fract(vec2(vary.z,vary.w));

    fColor = vec4(
        min(1.0 - step(ab.x*0.5, dist.x), 1.0 - step(ab.y*0.5, dist.y)),
        0.0,
        0.0,
        1.0
    );
}
