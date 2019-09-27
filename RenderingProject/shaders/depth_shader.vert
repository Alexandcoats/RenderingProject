#version 450

uniform mat4 p;
uniform mat4 v;
uniform mat4 subM = mat4(1.0);

in vec3 pos;
in mat4 m;

void main() {
	mat4 thisM = m * subM;

	gl_Position = p * v * thisM * vec4(pos, 1.0);
}