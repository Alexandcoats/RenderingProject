#version 450

uniform mat4 p;
uniform mat4 v;
uniform mat4 subM = mat4();

in vec3 pos;
in vec3 normal;
in vec2 texCoord;
in mat4 m;

out vec3 worldSpacePos;
out vec3 norm;
out vec2 UV;

void main() {
	thisM = m * subM;
	worldSpacePos = (thisM * vec4(pos, 1.0)).xyz;
	mat4 n = transpose(inverse(thisM));
	norm = (n * vec4(normalize(normal), 1.0)).xyz;
	UV = texCoord;

	gl_Position = p * v * vec4(worldSpacePos, 1.0);
}