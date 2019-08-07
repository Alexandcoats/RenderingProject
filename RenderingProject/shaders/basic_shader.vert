#version 450

uniform mat4 mvp;

in vec3 pos;
in vec3 normal;
in vec2 texCoord;

out vec3 worldPos;
out vec3 worldNormal;
out vec2 UV;

out gl_PerVertex{
	vec4 gl_Position;
};

void main() {
	worldPos = pos;
	worldNormal = normal;
	UV = texCoord;
    gl_Position = mvp * vec4(pos, 1.0);
}