#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform sampler2D texSampler;

in vec4 fragColor;

out vec4 outColor;

void main() {
    outColor = fragColor;
}