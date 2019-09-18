#version 450

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColor;
uniform sampler2D gMSSR;
uniform sampler2D gSASS;
uniform sampler2D gCC;

in vec2 UV;

out vec4 outColor;

void main() {
	outColor = texture(gColor, UV);
}