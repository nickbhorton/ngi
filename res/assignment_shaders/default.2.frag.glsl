#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform vec4 vary;

void main() {
    vec2 p = 2.0 * (uv - 0.5);
    vec2 o = vec2(vary.x,vary.y);
    fColor = vec4(
        fract(distance(p.x , o.x)),
        fract(distance(p.y , o.y)),
        0.0,
        1.0
    );
}
