#version 330

//variables d'entrées
in vec4 vLightSpacePos[7];
in vec3 vPosition_vs; //w0 normalize(-vPos)
in vec3 vNormal_vs;
in vec2 vTexCoords;
in vec3 vWorldPos;

out vec4 fFragColor;

struct Texture {
    sampler2D ka;
    sampler2D kd;
    sampler2D ks;

    float shininess;
    float opacity;
};

uniform Texture uTexture;

struct Light {
    vec3      position;
    int       type;
    vec3      color;
    float ufar_plane;
};

uniform Light uLight[7];

uniform sampler2D uShadowMap[7];
uniform samplerCube uShadowCubeMap[7];

uniform int uLightNB;

vec3 PointblinnPhong(int light) {
    float d = distance(vPosition_vs, uLight[light].position);
    vec3 Li = (uLight[light].color / (d * d));
    vec3 N = vNormal_vs;
    vec3 w0 = normalize(-vPosition_vs);
    vec3 wi = normalize(uLight[light].position - vPosition_vs);
    vec3 halfVector = (w0 + wi)/2.f;
    
    return Li*(texture(uTexture.kd, vTexCoords).xyz*max(dot(wi, N), 0.) + texture(uTexture.ks, vTexCoords).xyz*pow(max(dot(halfVector, N), 0.), uTexture.shininess));
}

vec3 blinnPhong(int light) {
    vec3 Li = uLight[light].color;
    vec3 N = normalize(vNormal_vs);
    vec3 w0 = normalize(-vPosition_vs);
    vec3 wi = normalize(uLight[light].position);
    vec3 halfVector = (w0 + wi)/2.f;
    
    return Li*(texture(uTexture.kd, vTexCoords).xyz*max(dot(wi, N), 0.) + texture(uTexture.ks, vTexCoords).xyz*max(pow(dot(halfVector, N), 0.), uTexture.shininess));
}

float calcShadowFactorPointLight(int light) {
    vec3 LightToVertex = vWorldPos - uLight[light].position;
    
    float Distance = length(LightToVertex)/uLight[light].ufar_plane;

    float SampledDistance = texture(uLight[light].shadowMapCube, LightToVertex).r;

    float bias = 0.025;

    if ((SampledDistance + bias) < Distance) {
        return 0.15;
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
    vec3 light = vec3(0);
    float shadow = 0;
    for (int i = 0; i < uLightNB; i++) {
        if (uLight[i].type == 0) {
            light += blinnPhong(i);
        }
        else light += PointblinnPhong(i);

        if (uLight[i].type == 2) {
            shadow = calcShadowFactorPointLight(i);
        }
        else {
            shadow = calcShadowFactorPCF(i);
        } 
    }
    shadow/=uLightNB;

    vec4 AmbientTexture = texture(uTexture.ka, vTexCoords)*0.5f;

    fFragColor = vec4(AmbientTexture.xyz + shadow*light, min(uTexture.opacity, AmbientTexture.w));
}
