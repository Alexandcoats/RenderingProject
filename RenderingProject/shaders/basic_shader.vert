#version 450
#extension GL_ARB_separate_shader_objects : enable

uniform mat4 mvp;

in vec3 pos;
in vec2 texCoord;

out vec2 fragTexCoord;

out gl_PerVertex{
	vec4 gl_Position;
};

void main() {
    gl_Position = mvp * vec4(pos, 1.0);
	fragTexCoord = texCoord;
}