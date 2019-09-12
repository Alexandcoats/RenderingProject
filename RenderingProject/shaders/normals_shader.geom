#version 450

layout(triangles) in;
layout(line_strip, max_vertices=2) out;

in vec3 norm[];

uniform mat4 p;
uniform mat4 v;
uniform mat4 n;
uniform mat4 m;

const float length = 2.0;

void main() {
	//gl_Position = p * v * m * gl_in[0].gl_Position;
	//EmitVertex();
	//gl_Position = p * v * m * gl_in[1].gl_Position;
	//EmitVertex();
	//gl_Position = p * v * m * gl_in[2].gl_Position;
	//EmitVertex();
	//gl_Position = p * v * m * gl_in[0].gl_Position;
	//EmitVertex();
	//EndPrimitive();

	vec3 P = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz)/3.0;
	vec3 N = (norm[0] + norm[1] + norm[2])/3.0;

	gl_Position = p * v * m * vec4(P, 1.0);
	EmitVertex();
	gl_Position = p * v * m * vec4(P + (N * length), 1.0);
	EmitVertex();
	EndPrimitive();

	//vec3 P0 = gl_in[0].gl_Position.xyz;
	//vec3 P1 = gl_in[1].gl_Position.xyz;
	//vec3 P2 = gl_in[2].gl_Position.xyz;

	//vec3 V0 = P0 - P1;
	//vec3 V1 = P2 - P1;

	//vec3 N = cross(V1, V0);
	//N = normalize(N);

	//vec3 P = (P0+P1+P2)/3.0;

	//gl_Position = p * v * m * vec4(P, 1.0);
	//EmitVertex();

	//gl_Position = p * v * m * vec4(P + N * length, 1.0);
	//EmitVertex();
	//EndPrimitive();
}