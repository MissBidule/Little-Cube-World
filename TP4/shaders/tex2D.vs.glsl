#version 330

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexCoordUV;

out vec2 vCoordUV;

uniform mat3 uModelMatrix;

void main() {
    
    vCoordUV = aVertexCoordUV;
    gl_Position = vec4((uModelMatrix * vec3(aVertexPosition, 1)).xy, 0, 1);
}
