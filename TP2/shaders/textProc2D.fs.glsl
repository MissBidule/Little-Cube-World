#version 330

in vec3 vColor;
in vec2 vPosition;

out vec4 fFragColor;

float tex1 = length(fract(5.0 * vPosition));
float tex2 = length(abs(fract(5.0 * vPosition) * 2.0 - 1.0));
float tex3 = mod(floor(10.0 * vPosition.x) + floor(10.0 * vPosition.y), 2.0);
float tex4 = smoothstep(0.3, 0.32, length(fract(5.0 * vPosition) - 0.5));
float tex5 = smoothstep(0.4, 0.5, max(abs(fract(8.0 * vPosition.x - 0.5 * mod(floor(8.0 * vPosition.y), 2.0)) - 0.5), abs(fract(8.0 * vPosition.y) - 0.5)));

float factor = tex5;

void main() {
    fFragColor = vec4(vColor * factor, 1);
}
