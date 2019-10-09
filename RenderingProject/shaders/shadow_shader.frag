#version 450

layout(early_fragment_tests) in;

uniform vec3 lightPos;
uniform int octant;
uniform int octSize;

layout (r32f) writeonly uniform image2D shadowMap;

in vec3 worldSpacePos;

float signNotZero(in float k) {
    return k >= 0.0 ? 1.0 : -1.0;
}

vec2 signNotZero(in vec2 v) {
    return vec2( signNotZero(v.x), signNotZero(v.y) );
}

vec2 octEncode(in vec3 v) {
    float l1norm = abs(v.x) + abs(v.y) + abs(v.z);
    vec2 result = v.xy * (1.0/l1norm);
    if (v.z < 0.0) {
        result = (1.0 - abs(result.yx)) * signNotZero(result.xy);
    }
	result.y *= -1.0;
    return result;
}

void main() {
	vec3 lightDir = normalize(worldSpacePos - lightPos);
	if		(octant == 0 && (lightDir.x > 0.0 || lightDir.z < 0.0 || lightDir.y < 0.0)) return;
	else if (octant == 1 && (lightDir.x > 0.0 || lightDir.z > 0.0 || lightDir.y < 0.0)) return;
	else if (octant == 2 && (lightDir.x < 0.0 || lightDir.z < 0.0 || lightDir.y < 0.0)) return;
	else if (octant == 3 && (lightDir.x < 0.0 || lightDir.z > 0.0 || lightDir.y < 0.0)) return;
	else if (octant == 4 && (lightDir.x < 0.0 || lightDir.z < 0.0 || lightDir.y > 0.0)) return;
	else if (octant == 5 && (lightDir.x < 0.0 || lightDir.z > 0.0 || lightDir.y > 0.0)) return;
	else if (octant == 6 && (lightDir.x > 0.0 || lightDir.z < 0.0 || lightDir.y > 0.0)) return;
	else if (octant == 7 && (lightDir.x > 0.0 || lightDir.z > 0.0 || lightDir.y > 0.0)) return;

	ivec2 depthCoord = ivec2(octSize * ((octEncode(lightDir) + 1.0) / 2.0));
	float depth = length(worldSpacePos - lightPos);
	imageStore(shadowMap, depthCoord, vec4(depth));
}