#version 330

//variables d'entrées
in vec4 vLightSpacePos[7];
in vec3 vPosition_vs; //w0 normalize(-vPos)
in vec3 vWPosition;
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

void main() {
    float distanceToShow = 7;
    float opacity = distanceToShow - distance(vWPosition, vec3(0));

    vec4 limitTexture = texture(uTexture.ka, vTexCoords)*0.5f;

    fFragColor = vec4(limitTexture.xyz,min(opacity, limitTexture.w));
}
