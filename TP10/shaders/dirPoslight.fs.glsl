#version 330

//variables d'entrées
in vec4 vLightSpacePos;
in vec3 vPosition_vs; //w0 normalize(-vPos)
in vec3 vNormal_vs;
in vec2 vTexCoords;
in vec3 vWorldPos;

out vec4 fFragColor;

uniform vec3 uKa;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;

uniform vec3 uLightDir_vs; //wi (need normalization)
uniform vec3 uLightPos_vs;
uniform vec3 uLightPos;
uniform vec3 uLightIntensity; //Li

uniform float ufar_plane;

//uniform sampler2D uTexture1;
uniform samplerCube uTexture;

vec3 PointblinnPhong() {
        float d = distance(vPosition_vs, uLightPos_vs);
        vec3 Li = (uLightIntensity / (d * d));
        vec3 N = vNormal_vs;
        vec3 w0 = normalize(-vPosition_vs);
        vec3 wi = normalize(uLightPos_vs - vPosition_vs);
        vec3 halfVector = (w0 + wi)/2.f;
        
        return Li*(uKd*max(dot(wi, N), 0.) + uKs*pow(max(dot(halfVector, N), 0.), uShininess));
}

vec3 blinnPhong() {
        vec3 Li = uLightIntensity;
        vec3 N = normalize(vNormal_vs);
        vec3 w0 = normalize(-vPosition_vs);
        vec3 wi = normalize(uLightDir_vs);
        vec3 halfVector = (w0 + wi)/2.f;
        
        return Li*(uKd*max(dot(wi, N), 0.) + uKs*max(pow(dot(halfVector, N), 0.), uShininess));
}

float calcShadowFactorPointLight() {
    vec3 LightToVertex = vWorldPos - uLightPos;
    
    float Distance = length(LightToVertex)/ufar_plane;

    //LightToVertex.y = -LightToVertex.y;

    float SampledDistance = texture(uTexture, LightToVertex).r;

    float bias = 0.025;

    if ((SampledDistance + bias) < Distance) {
        return 0.15;
    }
    else {
        return 1.0;
    }
}

void main() {
    fFragColor = vec4(vec3(texture(uTexture, vWorldPos - uLightPos).r), 1);//vec4(vec3(1-Distance/3.f), 1);
}
