#version 450

in vec2 pos;
in vec2 texCoord;

out vec2 UV;

void main() {
	gl_Position = vec4(pos, 0.0, 1.0);
	UV = texCoord;
}