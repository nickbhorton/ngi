#version 450 core

/*
this renders 3 instead of 2
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
    vec2 arr[34][3];
    arr[0][0] = vec2(0.481687, 1);
    arr[0][1] = vec2(0.625971, 1);
    arr[0][2] = vec2(0.73141, 0.966948);
    arr[1][0] = vec2(0.73141, 0.966948);
    arr[1][1] = vec2(0.836848, 0.933896);
    arr[1][2] = vec2(0.893452, 0.879044);
    arr[2][0] = vec2(0.893452, 0.879044);
    arr[2][1] = vec2(0.951165, 0.824191);
    arr[2][2] = vec2(0.951165, 0.758087);
    arr[3][0] = vec2(0.951165, 0.758087);
    arr[3][1] = vec2(0.951165, 0.699015);
    arr[3][2] = vec2(0.9101, 0.651196);
    arr[4][0] = vec2(0.9101, 0.651196);
    arr[4][1] = vec2(0.869034, 0.604079);
    arr[4][2] = vec2(0.801332, 0.573136);
    arr[5][0] = vec2(0.801332, 0.573136);
    arr[5][1] = vec2(0.733629, 0.542194);
    arr[5][2] = vec2(0.652608, 0.532349);
    arr[6][0] = vec2(0.652608, 0.532349);
    arr[6][1] = vec2(0.742508, 0.528129);
    arr[6][2] = vec2(0.82131, 0.5);
    arr[7][0] = vec2(0.82131, 0.5);
    arr[7][1] = vec2(0.901221, 0.472574);
    arr[7][2] = vec2(0.950055, 0.421238);
    arr[8][0] = vec2(0.950055, 0.421238);
    arr[8][1] = vec2(1, 0.369902);
    arr[8][2] = vec2(1, 0.295359);
    arr[9][0] = vec2(1, 0.295359);
    arr[9][1] = vec2(1, 0.211674);
    arr[9][2] = vec2(0.933407, 0.144163);
    arr[10][0] = vec2(0.933407, 0.144163);
    arr[10][1] = vec2(0.866815, 0.0773558);
    arr[10][2] = vec2(0.746948, 0.0386779);
    arr[11][0] = vec2(0.746948, 0.0386779);
    arr[11][1] = vec2(0.628191, 0);
    arr[11][2] = vec2(0.469478, 0);
    arr[12][0] = vec2(0.469478, 0);
    arr[12][1] = vec2(0.340733, 0);
    arr[12][2] = vec2(0.214206, 0.0309423);
    arr[13][0] = vec2(0.214206, 0.0309423);
    arr[13][1] = vec2(0.0876803, 0.0618847);
    arr[13][2] = vec2(0, 0.129395);
    arr[14][0] = vec2(0, 0.129395);
    arr[14][1] = vec2(0.0621532, 0.161744);
    arr[14][2] = vec2(0.125416, 0.194093);
    arr[15][0] = vec2(0.125416, 0.194093);
    arr[15][1] = vec2(0.190899, 0.14557);
    arr[15][2] = vec2(0.280799, 0.12166);
    arr[16][0] = vec2(0.280799, 0.12166);
    arr[16][1] = vec2(0.371809, 0.0977497);
    arr[16][2] = vec2(0.465039, 0.0977497);
    arr[17][0] = vec2(0.465039, 0.0977497);
    arr[17][1] = vec2(0.624861, 0.0977497);
    arr[17][2] = vec2(0.714761, 0.150492);
    arr[18][0] = vec2(0.714761, 0.150492);
    arr[18][1] = vec2(0.804662, 0.203938);
    arr[18][2] = vec2(0.804662, 0.295359);
    arr[19][0] = vec2(0.804662, 0.295359);
    arr[19][1] = vec2(0.804662, 0.398734);
    arr[19][2] = vec2(0.713651, 0.438819);
    arr[20][0] = vec2(0.713651, 0.438819);
    arr[20][1] = vec2(0.623751, 0.478903);
    arr[20][2] = vec2(0.488346, 0.478903);
    arr[21][0] = vec2(0.488346, 0.478903);
    arr[21][1] = vec2(0.423973, 0.478903);
    arr[21][2] = vec2(0.3596, 0.478903);
    arr[22][0] = vec2(0.3596, 0.478903);
    arr[22][1] = vec2(0.370699, 0.525316);
    arr[22][2] = vec2(0.382908, 0.572433);
    arr[23][0] = vec2(0.382908, 0.572433);
    arr[23][1] = vec2(0.427303, 0.572433);
    arr[23][2] = vec2(0.472808, 0.572433);
    arr[24][0] = vec2(0.472808, 0.572433);
    arr[24][1] = vec2(0.54717, 0.572433);
    arr[24][2] = vec2(0.612653, 0.590717);
    arr[25][0] = vec2(0.612653, 0.590717);
    arr[25][1] = vec2(0.679245, 0.609001);
    arr[25][2] = vec2(0.721421, 0.646273);
    arr[26][0] = vec2(0.721421, 0.646273);
    arr[26][1] = vec2(0.763596, 0.683544);
    arr[26][2] = vec2(0.763596, 0.74121);
    arr[27][0] = vec2(0.763596, 0.74121);
    arr[27][1] = vec2(0.763596, 0.821378);
    arr[27][2] = vec2(0.679245, 0.863572);
    arr[28][0] = vec2(0.679245, 0.863572);
    arr[28][1] = vec2(0.594895, 0.905767);
    arr[28][2] = vec2(0.475028, 0.905767);
    arr[29][0] = vec2(0.475028, 0.905767);
    arr[29][1] = vec2(0.375139, 0.905767);
    arr[29][2] = vec2(0.300777, 0.883263);
    arr[30][0] = vec2(0.300777, 0.883263);
    arr[30][1] = vec2(0.226415, 0.861463);
    arr[30][2] = vec2(0.150943, 0.816456);
    arr[31][0] = vec2(0.150943, 0.816456);
    arr[31][1] = vec2(0.0987791, 0.852321);
    arr[31][2] = vec2(0.0477248, 0.888889);
    arr[32][0] = vec2(0.0477248, 0.888889);
    arr[32][1] = vec2(0.145394, 0.947257);
    arr[32][2] = vec2(0.255272, 0.973277);
    arr[33][0] = vec2(0.255272, 0.973277);
    arr[33][1] = vec2(0.36515, 1);
    arr[33][2] = vec2(0.481687, 1);

    float min_dist = 3.402823466e+38;
    float min_t = 3.402823466e+38;
    int min_index = 0;

    vec2 P = uv;
    for (int i = 0; i < 34; i++) {
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
