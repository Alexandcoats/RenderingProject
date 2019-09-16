#version 450

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform mat4 n;

in vec3 pos;
in vec3 normal;
in vec2 texCoord;

out vec3 worldSpacePos;
out vec3 norm;
out vec2 UV;

void main() {
	worldSpacePos = (m * vec4(pos, 1.0)).xyz;
	norm = (n * vec4(normalize(normal), 1.0)).xyz;
	UV = texCoord;

	gl_Position = p * v * vec4(worldSpacePos, 1.0);
}