#version 450

uniform mat4 p;
uniform mat4 v;
uniform mat4 subM = mat4(1.0);

in vec3 pos;
in vec3 normal;
in vec2 texCoord;
in mat4 m;

out vec3 norm;
out vec2 UV;

void main() {
	mat4 thisM = m * subM;
	mat3 n = transpose(inverse(mat3(thisM)));
	norm = n * normalize(normal);
	UV = texCoord;

	gl_Position = p * v * thisM * vec4(pos, 1.0);
}