#version 330

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTexCoords;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aWeights;

const int MAXTAB = 4;

//Variables de sorties
out vec3 vPosition_vs;
out vec3 vNormal_vs;
out vec2 vTexCoords;
out vec4 vLightSpacePos[MAXTAB];
out vec3 vWorldPos;

//Matrices de transformations
uniform mat4 uMVPLight[MAXTAB];
uniform mat4 uMMatrix;
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

const int MAX_BONES = 100;
uniform mat4 uBoneTransforms[MAX_BONES];
uniform int uMovement;

void main() {
    mat4 BoneTransform = uBoneTransforms[aBoneIDs[0]] * aWeights[0];
    BoneTransform += uBoneTransforms[aBoneIDs[1]] * aWeights[1];
    BoneTransform += uBoneTransforms[aBoneIDs[2]] * aWeights[2];
    BoneTransform += uBoneTransforms[aBoneIDs[3]] * aWeights[3];

     if (uMovement == 0) BoneTransform = mat4(1);

    //Passage en coordonnées homogènes
    vec4 vertexPosition = BoneTransform * vec4(aVertexPosition, 1);
    vec4 vertexNormal = vec4(aVertexNormal, 0);
    
    //Calcul des variables de sorties
    vPosition_vs = vec3(uMVMatrix * vertexPosition);
    vNormal_vs = vec3(uNormalMatrix * vertexNormal);
    vTexCoords = aVertexTexCoords;
    vWorldPos = (uMMatrix * vertexPosition).xyz;
    for (int i = 0; i < MAXTAB ; i ++) {
        vLightSpacePos[i] = uMVPLight[i] * vertexPosition;
    }
    
    gl_Position = uMVPMatrix * vertexPosition;
}
