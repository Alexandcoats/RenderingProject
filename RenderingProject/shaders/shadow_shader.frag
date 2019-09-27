#version 450

layout(early_fragment_tests) in;

uniform vec3 lightPos;

layout (r32f) writeonly uniform image2D shadowMap;

in vec3 worldSpacePos;

vec2 octEncode(in vec3 v) {
    float l1norm = abs(v.x) + abs(v.y) + abs(v.z);
    vec2 result = v.xy * (1.0/l1norm);
    if (v.z < 0.0) {
        result = abs((1.0 - abs(result.yx)));
    }
    return result;
}

void main() {
	vec3 lightDir = lightPos - worldSpacePos;
	if(abs(lightDir.x) + abs(lightDir.y) > 1.0) return;
	ivec2 depthCoord = ivec2(512 * ((octEncode(lightDir) + 1.0) / 2.0));
	// Need to store only if part of this octant
	imageStore(shadowMap, depthCoord, vec4(gl_FragCoord.z));
}