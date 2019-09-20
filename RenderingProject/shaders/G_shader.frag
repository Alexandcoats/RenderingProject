#version 450

uniform sampler2D texSampler;

uniform float metallic = 0.0;
uniform float subsurface = 0.0;
uniform float specular = 0.0;
uniform float roughness = .9;
uniform float specularTint = 0.0;
uniform float anisotropic = 0.0;
uniform float sheen = 0.0;
uniform float sheenTint = 0.5;
uniform float clearcoat = 0.0;
uniform float clearcoatGloss = 1.0;

in vec3 norm;
in vec2 UV;

layout (location = 0) out vec3 gNormal;
layout (location = 1) out vec4 gColor;
layout (location = 2) out vec4 gMSSR;
layout (location = 3) out vec4 gSASS;
layout (location = 4) out vec2 gCC;

void main() {
	gNormal = norm;
	gColor = texture(texSampler, UV);
	gMSSR = vec4(metallic, subsurface, specular, roughness);
	gSASS = vec4(specularTint, anisotropic, sheen, sheenTint);
	gCC = vec2(clearcoat, clearcoatGloss);
}