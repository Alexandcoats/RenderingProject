#version 450

uniform mat4 p;
uniform mat4 v;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gMSSR;
uniform sampler2D gSASS;
uniform sampler2D gCC;
uniform sampler2D gDepth;

in vec2 UV;

out vec4 outColor;

vec3 worldPosFromDepth() {
    float z = texture(gDepth, UV).x * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(UV * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(p) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = inverse(v) * viewSpacePosition;

    return worldSpacePosition.xyz;
}

void main() {
	vec3 pos = worldPosFromDepth();
	outColor = vec4(pos, 1.0);
}