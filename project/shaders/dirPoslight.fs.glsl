#version 330

//variables d'entrées
in vec4 vLightSpacePos[10];
in vec3 vPosition_vs; //w0 normalize(-vPos)
in vec3 vNormal_vs;
in vec2 vTexCoords;

out vec4 fFragColor;

struct Color {
    vec3 ka;
    vec3 kd;
    vec3 ks;

    float shininess;
    float opacity;
};

uniform Color uColor;

struct Light {
    vec3      position;
    int       type;
    vec3      color;
    sampler2D shadowMap;
};

uniform Light uLight[10];

uniform int uLightNB;

vec3 PointblinnPhong(int light) {
        float d = distance(vPosition_vs, uLight[light].position);
        vec3 Li = (uLight[light].color / (d * d));
        vec3 N = vNormal_vs;
        vec3 w0 = normalize(-vPosition_vs);
        vec3 wi = normalize(uLight[light].position - vPosition_vs);
        vec3 halfVector = (w0 + wi)/2.f;
        
        return Li*(uColor.kd*max(dot(wi, N), 0.) + uColor.ks*pow(max(dot(halfVector, N), 0.), uColor.shininess));
}

vec3 blinnPhong(int light) {
        vec3 Li = uLight[light].color;
        vec3 N = normalize(vNormal_vs);
        vec3 w0 = normalize(-vPosition_vs);
        vec3 wi = normalize(uLight[light].position);
        vec3 halfVector = (w0 + wi)/2.f;
        
        return Li*(uColor.kd*max(dot(wi, N), 0.) + uColor.ks*max(pow(dot(halfVector, N), 0.), uColor.shininess));
}

float calcShadowFactorPCF(int light) {
    vec3 projCoords = vLightSpacePos[light].xyz / vLightSpacePos[light].w;

    if(projCoords.z > 1.0) {
        return 1.0;
    }

    vec3 UVCoords = 0.5 * projCoords + vec3(0.5);

    float DiffuseFactor = dot(normalize(vNormal_vs), -normalize(uLight[light].position));
    float bias = mix(0.005, 0.0, DiffuseFactor);

    //improvement : send the shadow size as uniform
    float TexelSize = 1/4096.f;
    //send as uniform the filter
    float ShadowMapFilterSize = 7.f;
    int HalfFilterSize = int(ShadowMapFilterSize / 2);
    
    float ShadowSum = 0.f;

    for (int y = -HalfFilterSize ; y < -HalfFilterSize + ShadowMapFilterSize; y++) {
        for (int x = -HalfFilterSize; x < -HalfFilterSize + ShadowMapFilterSize; x++) {
            vec2 Offset = vec2(x, y) * TexelSize;
            float depth = texture(uLight[light].shadowMap, UVCoords.xy + Offset).x;

            if (depth + bias < UVCoords.z)
                ShadowSum += 0.f;
            else
                ShadowSum += 1.f;
        }
    }
 
    float finalShadowFactor = ShadowSum / float(pow(ShadowMapFilterSize, 2));

    return finalShadowFactor;
}


void main() {
    vec3 lightAndShadow = vec3(0);
    for (int i = 0; i < uLightNB; i++) {
        if (uLight[i].type == 0) {
            lightAndShadow += calcShadowFactorPCF(i)*blinnPhong(i);
        }
        else lightAndShadow += calcShadowFactorPCF(i)*PointblinnPhong(i);
    }
    lightAndShadow/=uLightNB;
    fFragColor = vec4(uColor.ka + lightAndShadow, uColor.opacity);
}
