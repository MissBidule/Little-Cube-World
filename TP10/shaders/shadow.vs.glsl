#version 330

layout(location = 0) in vec3 aVertexPosition;

//Matrices de transformations
uniform mat4 uMVPLight;
uniform mat4 uMMatrix;

out vec3 vWorldPos;

void main() {
    vec4 Pos4 = vec4(aVertexPosition, 1);
    gl_Position = uMVPLight * Pos4;
    vWorldPos = (uMMatrix * Pos4).xyz;
}
