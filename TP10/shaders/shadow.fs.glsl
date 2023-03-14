#version 330

in vec3 vWorldPos;

uniform vec3 uLightPos;

out float fLightToPixelDistance;

void main() {

    // get distance between fragment and light source
    //fLightToPixelDistance = length(vWorldPos - uLightPos);

    fLightToPixelDistance = 1.f;
}
