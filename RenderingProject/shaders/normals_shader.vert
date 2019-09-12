#version 450

in vec3 pos;
in vec3 normal;
in vec2 texCoord;

out vec3 norm;

void main() {
	gl_Position = vec4(pos, 1.0);
	norm = normal;
}