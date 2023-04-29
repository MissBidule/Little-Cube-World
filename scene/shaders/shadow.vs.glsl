#version 330

layout(location = 0) in vec3 aVertexPosition;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aWeights;

//Variables de sorties
out vec3 vWorldPos;

//Matrices de transformations
uniform mat4 uMVPLight;
uniform mat4 uMMatrix;
uniform int uIsMoving;

const int MAX_BONES = 100;
uniform mat4 uBoneTransforms[MAX_BONES];

void main() {
    //Passage en coordonnées homogènes
    vec4 vertexPosition = vec4(aVertexPosition, 1);

    if (uIsMoving == 1)
    {
        mat4 BoneTransform = uBoneTransforms[aBoneIDs[0]] * aWeights[0];
        BoneTransform += uBoneTransforms[aBoneIDs[1]] * aWeights[1];
        BoneTransform += uBoneTransforms[aBoneIDs[2]] * aWeights[2];
        BoneTransform += uBoneTransforms[aBoneIDs[3]] * aWeights[3];

        vertexPosition = BoneTransform * vec4(aVertexPosition, 1);
    }
    
    //Calcul des variables de sorties
    vWorldPos = (uMMatrix * vertexPosition).xyz;
    
    gl_Position = uMVPLight * vertexPosition;
}
