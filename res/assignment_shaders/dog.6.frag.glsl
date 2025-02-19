#version 450 core

#define PI 3.1415926

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform vec2 img_res;
uniform sampler2D tex;

// dog stuff
uniform float sig;
uniform float k;
uniform float radius;

float gaussian(vec2 v, float sigma) {
    float x = v.x;
    float y = v.y;
    float sig_sqr = pow(sigma,2.0);
    return (1.0 / (2.0 * PI * sig_sqr)) * 
        exp(-(pow(x, 2.0) + pow(y, 2.0)) / (2.0 * sig_sqr));
}

void main() {
    float color_sum = 0.0;
    for(float x = -radius; x <= radius; x += 1){
        for(float y = -radius; y <= radius; y += 1){
            vec2 pixel_offset = vec2(x,y);
            vec2 uv_offset = pixel_offset / img_res.xy;
            float g1 = gaussian(pixel_offset, sig) * length(texture(tex, uv + uv_offset));
            float g2 = gaussian(pixel_offset, k * sig) * length(texture(tex, uv + uv_offset));
            color_sum += g1 - g2;
        }   
    }
    fColor = vec4(vec3(color_sum), 1.0);
}
