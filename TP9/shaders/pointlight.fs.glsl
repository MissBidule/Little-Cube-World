#version 330

//variables d'entrées
in vec3 vPosition_vs; //w0 normalize(-vPos)
in vec3 vNormal_vs;
in vec2 vTexCoords;

out vec4 fFragColor;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightPos_vs; //wi (need normalization and pos)
uniform vec3 uLightIntensity; //Li

vec3 blinnPhong() {
        float d = distance(vPosition_vs, uLightPos_vs);
        vec3 Li = (uLightIntensity / (d * d));
        vec3 N = vNormal_vs;
        vec3 w0 = normalize(-vPosition_vs);
        vec3 wi = normalize(uLightPos_vs - vPosition_vs);
        vec3 halfVector = (w0 + wi)/2.f;
        
        return Li*(uKd*dot(wi, N) + uKs*pow(dot(halfVector, N), uShininess));
}

void main() {
    fFragColor = vec4(blinnPhong(), 1);
}
