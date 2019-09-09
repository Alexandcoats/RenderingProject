#version 450

uniform mat4 fr;
uniform mat4 m;
uniform mat4 vp;

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
	worldPos = (m * vec4(pos, 1.0)).xyz;
	worldNormal = normalize(fr * vec4(normal, 1.0)).xyz;
	UV = texCoord;
    gl_Position = vp * m * vec4(pos, 1.0);
}