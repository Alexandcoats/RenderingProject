#version 450

uniform mat4 mvp;

in vec3 pos;
in vec2 texCoord;

out vec2 UV;

out gl_PerVertex{
	vec4 gl_Position;
};

void main() {
	UV = texCoord;
    gl_Position = mvp * vec4(pos, 1.0);
}