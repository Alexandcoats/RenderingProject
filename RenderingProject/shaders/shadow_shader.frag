#version 450

struct Light {
	vec3 pos;
	vec3 color;
	float brightness;
};
uniform Light light;

layout (location = 0) out vec3 shadowDepth;

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
    return result;
}

void main() {
	
}