#version 330

layout(location = 0) in vec3 aVertexPosition;

//Matrices de transformations
uniform mat4 uMVPLight;

void main() {
    gl_Position = uMVPLight * vec4(aVertexPosition, 1);
}
