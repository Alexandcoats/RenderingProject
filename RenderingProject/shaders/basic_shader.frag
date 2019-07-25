#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform sampler2D texSampler;

in vec2 texCoord;

out vec4 outColor;

void main() {
    outColor = texture(texSampler, texCoord);
}