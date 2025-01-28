#version 450 core

/*
changed as many divisions as I could
*/ 

layout (location = 0) out vec4 fColor;

in vec2 uv;

vec2 quad_evaluate(vec2 P0, vec2 p1, vec2 p2, float t) {
    return (t * t * p2) + (2.0 * t * p1) + P0;
}

vec2 quad_derivative(vec2 P0, vec2 P1, vec2 P2, float t) {
    return 2.0 * t * (P2 - 2.0 * P1 + P0) + 2.0 * (P1 - P0);
}

vec3 solve_cubic(float a, float b, float c, float d) {
    float M_PI = 3.141592653589793;
    float a_inv = 1.0/a;
    b = b * a_inv;
    c = c * a_inv;
    d = d * a_inv;

    // 1/9
    float b_sqr = b * b;
    float q = (3.0 * c - (b_sqr)) * 0.1111111111111111;
    float q_exp3 = q * q * q;
    float r = -(27.0 * d) + b * (9.0 * c - 2.0 * (b_sqr));
    // 1/54
    r = r * 0.01851851852;

    // we only care about roots between 0 and 1
    float ret1r = 2.0;
    float ret2r = 2.0;
    float ret3r = 2.0;

    float disc = q_exp3 + r * r;
    // 1/3
    float one_third = 0.333333333333333;
    float term1 = (b * one_third);

    float s;
    float t;
    if (disc > 0.0) { // one root real, two are complex
        s = r + sqrt(disc);
        if (s < 0.0) {
            s = -pow(-s, one_third);
        }
        else {
            s = pow(s, one_third);
        }

        t = r - sqrt(disc);
        if (t < 0.0) {
            t = -pow(-t, one_third);
        }
        else {
            t = pow(t, one_third);
        }
        ret1r = -term1 + s + t;
    } 
    else {
        // The remaining options are all real

        if (disc == 0.0) { // All roots real, at least two are equal.
            float r13;
            if (r < 0.0) {
                r13 = -pow(-r, one_third);
            }
            else {
                r13 = pow(r, one_third);
            }
            ret1r = -term1 + 2.0 * r13;
            ret2r = -(r13 + term1);
            ret3r = ret2r;
        }
        else { // Only option left is that all roots are real and unequal (to get here, q < 0)
            q = -q;
            float dum1 = q * q * q;
            dum1 = acos(r / sqrt(dum1));
            float r13 = 2.0 * sqrt(q);
            ret1r = -term1 + r13 * cos(dum1 * one_third);
            ret2r = -term1 + r13 * cos((dum1 + 2.0 * M_PI) * one_third);
            ret3r = -term1 + r13 * cos((dum1 + 4.0 * M_PI) * one_third);
        }
    }
    return vec3(ret1r, ret2r, ret3r);
}

void main() {

    vec2 arr[22][3];
    arr[0][0] = vec2(0.485023, 1.0);
    arr[0][1] = vec2(0.634793, 1.0);
    arr[0][2] = vec2(0.741935, 0.963545);
    arr[1][0] = vec2(0.741935, 0.963545);
    arr[1][1] = vec2(0.849078, 0.927806);
    arr[1][2] = vec2(0.90553, 0.865618);
    arr[2][0] = vec2(0.90553, 0.865618);
    arr[2][1] = vec2(0.963134, 0.803431);
    arr[2][2] = vec2(0.963134, 0.725518);
    arr[3][0] = vec2(0.963134, 0.725518);
    arr[3][1] = vec2(0.963134, 0.657613);
    arr[3][2] = vec2(0.925115, 0.592566);
    arr[4][0] = vec2(0.925115, 0.592566);
    arr[4][1] = vec2(0.887097, 0.528234);
    arr[4][2] = vec2(0.802995, 0.456755);
    arr[5][0] = vec2(0.802995, 0.456755);
    arr[5][1] = vec2(0.720046, 0.38599);
    arr[5][2] = vec2(0.584101, 0.2995);
    arr[6][0] = vec2(0.584101, 0.2995);
    arr[6][1] = vec2(0.448157, 0.213009);
    arr[6][2] = vec2(0.252304, 0.102216);
    arr[7][0] = vec2(0.252304, 0.102216);
    arr[7][1] = vec2(0.625576, 0.102216);
    arr[7][2] = vec2(1.0, 0.102216);
    arr[8][0] = vec2(1.0, 0.102216);
    arr[8][1] = vec2(0.987327, 0.0507505);
    arr[8][2] = vec2(0.975806, 0.0);
    arr[9][0] = vec2(0.975806, 0.0);
    arr[9][1] = vec2(0.503456, 0.0);
    arr[9][2] = vec2(0.0322581, 0.0);
    arr[10][0] = vec2(0.0322581, 0.0);
    arr[10][1] = vec2(0.0322581, 0.0486062);
    arr[10][2] = vec2(0.0322581, 0.0979271);
    arr[11][0] = vec2(0.0322581, 0.0979271);
    arr[11][1] = vec2(0.256912, 0.231594);
    arr[11][2] = vec2(0.398618, 0.323803);
    arr[12][0] = vec2(0.398618, 0.323803);
    arr[12][1] = vec2(0.541475, 0.416011);
    arr[12][2] = vec2(0.619816, 0.482487);
    arr[13][0] = vec2(0.619816, 0.482487);
    arr[13][1] = vec2(0.698157, 0.549678);
    arr[13][2] = vec2(0.728111, 0.605432);
    arr[14][0] = vec2(0.728111, 0.605432);
    arr[14][1] = vec2(0.759217, 0.661187);
    arr[14][2] = vec2(0.759217, 0.721229);
    arr[15][0] = vec2(0.759217, 0.721229);
    arr[15][1] = vec2(0.759217, 0.804146);
    arr[15][2] = vec2(0.684332, 0.852037);
    arr[16][0] = vec2(0.684332, 0.852037);
    arr[16][1] = vec2(0.609447, 0.899929);
    arr[16][2] = vec2(0.475806, 0.899929);
    arr[17][0] = vec2(0.475806, 0.899929);
    arr[17][1] = vec2(0.358295, 0.899929);
    arr[17][2] = vec2(0.28341, 0.874911);
    arr[18][0] = vec2(0.28341, 0.874911);
    arr[18][1] = vec2(0.208525, 0.849893);
    arr[18][2] = vec2(0.133641, 0.794853);
    arr[19][0] = vec2(0.133641, 0.794853);
    arr[19][1] = vec2(0.0668203, 0.827019);
    arr[19][2] = vec2(0.0, 0.859185);
    arr[20][0] = vec2(0.0, 0.859185);
    arr[20][1] = vec2(0.0944701, 0.930665);
    arr[20][2] = vec2(0.208525, 0.964975);
    arr[21][0] = vec2(0.208525, 0.964975);
    arr[21][1] = vec2(0.323733, 1.0);
    arr[21][2] = vec2(0.485023, 1.0);

    float min_dist = 3.402823466e+38;
    float min_t = 3.402823466e+38;
    int min_index = 0;

    vec2 P = uv;
    for (int i = 0; i < 22; i++) {
        vec2 P0 = arr[i][0];
        vec2 P1 = arr[i][1];
        vec2 P2 = arr[i][2];

        vec2 p = P - P0;
        vec2 p1 = P1 - P0;
        vec2 p2 = P2 - 2.0 * P1 + P0;

        float a = dot(p2, p2);
        float b = 3.0 * dot(p1, p2);
        float c = 2.0 * dot(p1, p1) - dot(p2, p);
        float d = -dot(p1, p);
        vec3 ts = solve_cubic(a, b, c, d);

        float dist;
        dist = distance(quad_evaluate(P0, p1, p2, 0.0), P);
        if (dist < min_dist) {
            min_dist = dist;
            min_t = 0.0;
            min_index = i;
        }

        dist = distance(quad_evaluate(P0, p1, p2, 1.0), P);
        if (dist < min_dist) {
            min_dist = dist;
            min_t = 1.0;
            min_index = i;
        }
        if (ts[0] > 0.0 && ts[0] < 1.0) {
            dist = distance(quad_evaluate(P0, p1, p2, ts[0]), P);
            if (dist < min_dist) {
                min_dist = dist;
                min_t = ts[0];
                min_index = i;
            }
        }
        if (ts[1] > 0.0 && ts[1] < 1.0) {
            dist = distance(quad_evaluate(P0, p1, p2, ts[1]), P);
            if (dist < min_dist) {
                min_dist = dist;
                min_t = ts[1];
                min_index = i;
            }
        }
        if (ts[2] > 0.0 && ts[2] < 1.0) {
            dist = distance(quad_evaluate(P0, p1, p2, ts[2]), P);
            if (dist < min_dist) {
                min_dist = dist;
                min_t = ts[2];
                min_index = i;
            }
        }
    }

    vec2 P0 = arr[min_index][0];
    vec2 P1 = arr[min_index][1];
    vec2 P2 = arr[min_index][2];

    vec2 p = P - P0;
    vec2 p1 = P1 - P0;
    vec2 p2 = P2 - 2.0 * P1 + P0;

    vec2 dt = quad_derivative(P0, P1, P2, min_t);
    vec2 bntmp = quad_evaluate(P0, p1, p2, min_t) - P;

    vec3 cross1 = vec3(dt[0], dt[1], 0.0);
    vec3 cross2 = vec3(bntmp[0], bntmp[1], 0.0);
    vec3 crossed = cross(cross1,cross2);
    if (crossed.z < 0.0) {
        fColor = vec4(
            vec3(0.0,0.0,1.0),
            1.0
        );
    }
    else {
        fColor = vec4(
            vec3(1.0,0.0,0.0),
            1.0
        );
    }
}
