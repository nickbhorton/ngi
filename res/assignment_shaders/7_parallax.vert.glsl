#version 450 core

layout (location = 0) in vec4 vPosition;
layout (location = 1) in vec4 vUvs;
layout (location = 2) in vec4 vNorm;
layout (location = 3) in vec4 vTang;

out vec2 uv;
out vec3 frag_pos;
out mat3 tbn;

uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * vPosition;
    frag_pos = vec3(vPosition);
    uv = vUvs.xy;
    vec3 N = normalize(vNorm.xyz);
    vec3 T = normalize(vTang.xyz);
    vec3 B = normalize(cross(N, T));
    tbn = transpose(mat3(T,B,N));
}
