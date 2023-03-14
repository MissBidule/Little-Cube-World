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

// float calcShadowFactor() {
//     vec3 projCoords = vLightSpacePos.xyz / vLightSpacePos.w;
//     vec2 UVCoords = vec2(0.5 * projCoords.x + 0.5, 0.5 * projCoords.y + 0.5);
//     float z = 0.5 * projCoords.z + 0.5;
//     float depth = texture(uTexture1, UVCoords).x;

//     float bias = 0.015;

//     if (depth + bias < z)
//         return 0.5;
//     else
//         return 1.0;
// }

float calcShadowFactorPointLight() {
    vec3 LightToVertex = vWorldPos - uLightPos;
    
    float Distance = length(LightToVertex);

    //LightToVertex.y = -LightToVertex.y;

    float SampledDistance = max(texture(uTexture, LightToVertex).r, 0);

    float bias = 0.015;

    if ((SampledDistance + bias) < Distance) {
         return 0.15;
    }
    else {
        return 1.0;
    }
}

vec3 LightToVertex = vWorldPos - uLightPos;
    
    float Distance = length(LightToVertex);
    vec3 LightToVertex2 = vec3(LightToVertex.x, -LightToVertex.y, LightToVertex.z);

    float SampledDistance = (texture(uTexture, LightToVertex2).r);


void main() {
    fFragColor = vec4(vec3(SampledDistance > 1000), 1);//vec4(vec3(1-Distance/3.f), 1);
}
