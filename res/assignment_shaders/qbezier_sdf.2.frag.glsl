#version 450 core

layout (location = 0) out vec4 fColor;

in vec2 uv;

uniform vec4 vary;
uniform vec2 vary2;

vec2 quad_evaluate(vec2 P0, vec2 p1, vec2 p2, float t) {
    return (t * t * p2) + (2.0 * t * p1) + P0;
}

vec2 quad_derivative(vec2 P0, vec2 P1, vec2 P2, float t) {
    return 2.0 * t * (P2 - 2.0 * P1 + P0) + 2.0 * (P1 - P0);
}

vec3 solve_cubic(float a, float b, float c, float d) {
    float M_PI = 3.141592653589793;
    b = b / a;
    c = c / a;
    d = d / a;

    float q = (3.0 * c - (b * b)) / 9.0;
    float r = -(27.0 * d) + b * (9.0 * c - 2.0 * (b * b));
    r = r / 54.0;

    // we only care about roots between 0 and 1
    float ret1r = 2.0;
    float ret2r = 2.0;
    float ret3r = 2.0;

    float disc = q * q * q + r * r;
    float term1 = (b / 3.0);

    float s;
    float t;
    if (disc > 0.0) { // one root real, two are complex
        s = r + sqrt(disc);
        if (s < 0.0) {
            s = -pow(-s, 1.0 / 3.0);
        }
        else {
            s = pow(s, 1.0 / 3.0);
        }

        t = r - sqrt(disc);
        if (t < 0.0) {
            t = -pow(-t, 1.0 / 3.0);
        }
        else {
            t = pow(t, 1.0 / 3.0);
        }
        ret1r = -term1 + s + t;
    } 
    else {
        // The remaining options are all real

        if (disc == 0.0) { // All roots real, at least two are equal.
            float r13;
            if (r < 0.0) {
                r13 = -pow(-r, 1.0 / 3.0);
            }
            else {
                r13 = pow(r, 1.0 / 3.0);
            }
            ret1r = -term1 + 2.0 * r13;
            ret2r = -(r13 + term1);
            ret3r = -(r13 + term1);
        }
        else { // Only option left is that all roots are real and unequal (to get here, q < 0)
            q = -q;
            float dum1 = q * q * q;
            dum1 = acos(r / sqrt(dum1));
            float r13 = 2.0 * sqrt(q);
            ret1r = -term1 + r13 * cos(dum1 / 3.0);
            ret2r = -term1 + r13 * cos((dum1 + 2.0 * M_PI) / 3.0);
            ret3r = -term1 + r13 * cos((dum1 + 4.0 * M_PI) / 3.0);
        }
    }
    return vec3(ret1r, ret2r, ret3r);
}

void main() {
    vec2 P0 = vec2(vary.x, vary.y);
    vec2 P1 = vec2(vary2.x,vary2.y);
    vec2 P2 = vec2(vary.z, vary.w);
    vec2 P = uv;

    vec2 p = P - P0;
    vec2 p1 = P1 - P0;
    vec2 p2 = P2 - 2.0 * P1 + P0;

    float a = dot(p2, p2);
    float b = 3.0 * dot(p1, p2);
    float c = 2.0 * dot(p1, p1) - dot(p2, p);
    float d = -dot(p1, p);
    vec3 ts = solve_cubic(a, b, c, d);

    float min_dist = 3.402823466e+38;
    float min_t = 3.402823466e+38;

    float dist;
    dist = distance(quad_evaluate(P0, p1, p2, 0.0), P);
    if (dist < min_dist) {
        min_dist = dist;
        min_t = 0.0;
    }

    dist = distance(quad_evaluate(P0, p1, p2, 1.0), P);
    if (dist < min_dist) {
        min_dist = dist;
        min_t = 1.0;
    }
    if (ts[0] > 0.0 && ts[0] < 1.0) {
        dist = distance(quad_evaluate(P0, p1, p2, ts[0]), P);
        if (dist < min_dist) {
            min_dist = dist;
            min_t = ts[0];
        }
    }
    if (ts[1] > 0.0 && ts[1] < 1.0) {
        dist = distance(quad_evaluate(P0, p1, p2, ts[1]), P);
        if (dist < min_dist) {
            min_dist = dist;
            min_t = ts[1];
        }
    }
    if (ts[2] > 0.0 && ts[2] < 1.0) {
        dist = distance(quad_evaluate(P0, p1, p2, ts[2]), P);
        if (dist < min_dist) {
            min_dist = dist;
            min_t = ts[2];
        }
    }

    vec2 dt = quad_derivative(P0, P1, P2, min_t);
    vec2 bntmp = quad_evaluate(P0, p1, p2, min_t) - P;

    vec3 cross1 = vec3(dt[0], dt[1], 0.0);
    vec3 cross2 = vec3(bntmp[0], bntmp[1], 0.0);
    vec3 crossed = cross(cross1,cross2);
    if (crossed.z < 0.0) {
        fColor = vec4(
            vec3(0.0,0.0,min_dist),
            1.0
        );
    }
    else {
        fColor = vec4(
            vec3(min_dist,0.0,0.0),
            1.0
        );
    }
}
