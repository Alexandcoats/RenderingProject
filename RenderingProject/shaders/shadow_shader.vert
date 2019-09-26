#version 450

uniform mat4 p;
uniform mat4 v;
uniform mat4 subM = mat4(1.0);

in vec3 pos;
in mat4 m;

out vec3 worldSpacePos;

void main() {
	mat4 thisM = m * subM;

	worldSpacePos = vec3(thisM * vec4(pos, 1.0));

	gl_Position = p * v * thisM * vec4(pos, 1.0);
}