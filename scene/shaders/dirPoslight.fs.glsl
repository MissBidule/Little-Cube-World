#version 330

const int MAXTAB = 3;

//variables d'entrées
in vec4 vLightSpacePos[MAXTAB];
in vec3 vPosition_vs; //w0 normalize(-vPos)
in vec3 vNormal_vs;
in vec2 vTexCoords;
in vec3 vWorldPos;

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
    vec3      position_vs;
    int       type;
    vec3      color;
    float ufar_plane;
};

uniform Light uLight[MAXTAB];

uniform sampler2D uShadowMap_0;
uniform sampler2D uShadowMap_1;
uniform sampler2D uShadowMap_2;

uniform samplerCube uShadowCubeMap_0;
uniform samplerCube uShadowCubeMap_1;
uniform samplerCube uShadowCubeMap_2;

uniform int uLightNB;

vec3 PointblinnPhong(int light) {
        float d = distance(vPosition_vs, uLight[light].position_vs);
        vec3 Li = (uLight[light].color / (d * d));
        vec3 N = vNormal_vs;
        vec3 w0 = normalize(-vPosition_vs);
        vec3 wi = normalize(uLight[light].position_vs - vPosition_vs);
        vec3 halfVector = (w0 + wi)/2.f;
        
        return Li*(uColor.kd*max(dot(wi, N), 0.) + uColor.ks*pow(max(dot(halfVector, N), 0.), uColor.shininess));
}

vec3 blinnPhong(int light) {
        vec3 Li = uLight[light].color;
        vec3 N = normalize(vNormal_vs);
        vec3 w0 = normalize(-vPosition_vs);
        vec3 wi = normalize(uLight[light].position_vs);
        vec3 halfVector = (w0 + wi)/2.f;
        
        return Li*(uColor.kd*max(dot(wi, N), 0.) + uColor.ks*max(pow(dot(halfVector, N), 0.), uColor.shininess));
        return vec3(0);
}

float calcShadowFactorPointLight(int light) {

    vec3 LightToVertex = vWorldPos - uLight[light].position;
    
    float Distance = length(LightToVertex)/uLight[light].ufar_plane;

    float SampledDistance = 0;//texture(uShadowCubeMap_0, LightToVertex).r;

    switch (light) {
    case 0:
        SampledDistance = texture(uShadowCubeMap_0, LightToVertex).r;
        break;
    case 1:
        SampledDistance = texture(uShadowCubeMap_1, LightToVertex).r;
        break;
    case 2:
        SampledDistance = texture(uShadowCubeMap_2, LightToVertex).r;
        break;
    }

    float bias = 0.025;

    if ((SampledDistance + bias) < Distance) {
        return 0.f;
    }
    else {
        return 1.0;
    }
}

float calcShadowFactorPCF(int light) {

    vec3 projCoords = vLightSpacePos[light].xyz / vLightSpacePos[light].w;

    if(projCoords.z > 1.0) {
        return 1.0;
    }

    vec3 UVCoords = 0.5 * projCoords + vec3(0.5);

    float DiffuseFactor = dot(normalize(vNormal_vs), -normalize(uLight[light].position_vs));
    float bias = mix(0.005, 0.0, DiffuseFactor);

    //improvement : send the shadow size as uniform
    float TexelSize = 1/4096.f;
    //improvement : send the filter as uniform 
    float ShadowMapFilterSize = 13.f;
    int HalfFilterSize = int(ShadowMapFilterSize / 2);
    
    float ShadowSum = 0.f;

    for (int y = -HalfFilterSize ; y < -HalfFilterSize + ShadowMapFilterSize; y++) {
        for (int x = -HalfFilterSize; x < -HalfFilterSize + ShadowMapFilterSize; x++) {
            vec2 Offset = vec2(x, y) * TexelSize;
            float depth = 0;//texture(uShadowMap_0, UVCoords.xy + Offset).x;

            switch (light) {
            case 0:
                depth = texture(uShadowMap_0, UVCoords.xy + Offset).x;
                break;
            case 1:
                depth = texture(uShadowMap_1, UVCoords.xy + Offset).x;
                break;
            case 2:
                depth = texture(uShadowMap_2, UVCoords.xy + Offset).x;
                break;
            }

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
    vec3 light = vec3(0);
    vec3 maxLight = vec3(0);
    for (int i = 0; i < uLightNB; i++) {

        //the multiple ifs prevent lag (idk why)
        //calcShadowFactor[...](i) is laggy
        if (uLight[i].type == 0) {
            if (i == 0) light = calcShadowFactorPCF(0) * blinnPhong(0);
            if (i == 1) light = calcShadowFactorPCF(1) * blinnPhong(1);
            if (i == 2) light = calcShadowFactorPCF(2) * blinnPhong(2);
        }
        else if (uLight[i].type == 1) {
            if (i == 0) light = calcShadowFactorPCF(0) * PointblinnPhong(0);
            if (i == 1) light = calcShadowFactorPCF(1) * PointblinnPhong(1);
            if (i == 2) light = calcShadowFactorPCF(2) * PointblinnPhong(2);
        }
        else {
            if (i == 0) light = calcShadowFactorPointLight(0) * PointblinnPhong(0);
            if (i == 1) light = calcShadowFactorPointLight(1) * PointblinnPhong(1);
            if (i == 2) light = calcShadowFactorPointLight(2) * PointblinnPhong(2);
        }

        maxLight = vec3(max(maxLight.x, light.x), max(maxLight.y, light.y), max(maxLight.z, light.z));
    }

    fFragColor = vec4(uColor.ka + maxLight,uColor.opacity);
}