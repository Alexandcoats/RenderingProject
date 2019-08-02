#version 450

uniform sampler2D texSampler;

in vec2 UV;

out vec4 outColor;

void main() {
    outColor = texture(texSampler, UV);
}