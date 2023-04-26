#version 330 core

out vec4 fFragColor;

struct Color {
    vec3 ka;
    vec3 kd;
    vec3 ks;

    float shininess;
    float opacity;
};

uniform Color uColor;
uniform int uLightNB;

void main(){
    fFragColor = vec4(uColor.ka, uColor.opacity);
}
