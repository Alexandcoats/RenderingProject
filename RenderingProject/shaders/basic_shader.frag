#version 450

uniform sampler2D texSampler;

in vec4 fragColor;

out vec4 outColor;

void main() {
    outColor = fragColor;
}