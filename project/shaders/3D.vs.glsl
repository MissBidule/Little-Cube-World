#version 330

layout(location = 0) in vec3 aVertexPosition;
layout(location = 1) in vec3 aVertexNormal;
layout(location = 2) in vec2 aVertexTexCoords;

//Variables de sorties
out vec3 vPosition_vs;
out vec3 vNormal_vs;
out vec2 vTexCoords;
out vec4 vLightSpacePos[10];

//Matrices de transformations
uniform mat4 uMVPLight[10];
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

void main() {
    //Passage en coordonnées homogènes
    vec4 vertexPosition = vec4(aVertexPosition, 1);
    vec4 vertexNormal = vec4(aVertexNormal, 0);
    
    //Calcul des variables de sorties
    vPosition_vs = vec3(uMVMatrix * vertexPosition);
    vNormal_vs = vec3(uNormalMatrix * vertexNormal);
    vTexCoords = aVertexTexCoords;
    for (int i = 0; i < 10 ; i ++) {
        vLightSpacePos[i] = uMVPLight[i] * vertexPosition;
    }
    
    gl_Position = uMVPMatrix * vertexPosition;
}
