#version 330

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vColor;
out vec2 vPosition;

void main() {
    vColor = aVertexColor;
    vPosition = aVertexPosition;
    gl_Position = vec4(aVertexPosition, 0, 1);
}
