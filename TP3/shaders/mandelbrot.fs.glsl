#version 330

in vec2 vPosition;

out vec4 fFragColor;

const int NMAX = 100;

vec2 complexSqr(vec2 z) {
    
    return vec2(pow(z.x,2) - pow(z.y, 2), 2*z.x*z.y);
}

void main() {
    vec2 z = vPosition;
    int i;
    for (i = 1; i <= NMAX; i++) {
        z = complexSqr(z) + vPosition;
        if (length(z) > 2) break;
    }
    fFragColor = vec4(i/float(NMAX), 0., 0., 1);
}
