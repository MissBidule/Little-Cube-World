#version 330

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTexCoords;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aWeights;

//Variables de sorties
out vec3 vPosition_vs;
out vec3 vNormal_vs;
out vec2 vTexCoords;
out vec4 vLightSpacePos;
flat out ivec4 vBoneIDs;
out vec4 vWeights;

//Matrices de transformations
uniform mat4 uMVPLight;
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

const int MAX_BONES = 100;
uniform mat4 uBoneTransforms[MAX_BONES];

void main() {
    mat4 BoneTransform = uBoneTransforms[aBoneIDs[0]] * aWeights[0];
    BoneTransform += uBoneTransforms[aBoneIDs[1]] * aWeights[1];
    BoneTransform += uBoneTransforms[aBoneIDs[2]] * aWeights[2];
    BoneTransform += uBoneTransforms[aBoneIDs[3]] * aWeights[3];

    //Passage en coordonnées homogènes
    vec4 vertexPosition = BoneTransform * vec4(aVertexPosition, 1);
    vec4 vertexNormal = vec4(aVertexNormal, 0);
    
    //Calcul des variables de sorties
    vPosition_vs = vec3(uMVMatrix * vertexPosition);
    vNormal_vs = vec3(uNormalMatrix * vertexNormal);
    vTexCoords = aVertexTexCoords;
    vLightSpacePos = uMVPLight * vertexPosition;
    vBoneIDs = aBoneIDs;
    vWeights = aWeights;
    
    gl_Position = uMVPMatrix * vertexPosition;
}
