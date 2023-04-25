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

uniform sampler2D uShadowMap_0;
uniform sampler2D uShadowMap_1;
uniform sampler2D uShadowMap_2;
uniform sampler2D uShadowMap_3;
uniform sampler2D uShadowMap_4;
uniform sampler2D uShadowMap_5;
uniform sampler2D uShadowMap_6;

uniform samplerCube uShadowCubeMap_0;
uniform samplerCube uShadowCubeMap_1;
uniform samplerCube uShadowCubeMap_2;
uniform samplerCube uShadowCubeMap_3;
uniform samplerCube uShadowCubeMap_4;
uniform samplerCube uShadowCubeMap_5;
uniform samplerCube uShadowCubeMap_6;

uniform int uLightNB;

void main() {
    float distanceToShow = 13;
    float opacity = distanceToShow - distance(vWPosition, vec3(0));

    vec4 limitTexture = texture(uTexture.kd, vTexCoords)*0.5f;

    fFragColor = vec4(limitTexture.xyz,min(opacity, limitTexture.w));
}
