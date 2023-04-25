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

#version 330

in vec3 vWorldPos;

uniform vec3 uLightPos;
uniform float ufar_plane;

void main() {

    // get distance between fragment and light source
    float lightDistance = distance(vWorldPos, uLightPos);

    lightDistance = lightDistance / ufar_plane;

    gl_FragDepth = lightDistance;
}
