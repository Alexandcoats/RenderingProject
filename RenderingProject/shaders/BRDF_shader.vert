#version 450

uniform mat4 p;
uniform mat4 v;
uniform mat4 m;
uniform mat4 n;

in vec3 pos;
in vec3 normal;
in vec2 texCoord;

out vec3 worldSpacePos;
out vec3 worldSpaceNormal;
out vec3 worldSpaceTangent;
out vec3 worldSpaceBitangent;
out vec2 UV;

void computeTangentVectors(vec3 inVec, out vec3 uVec, out vec3 vVec) {
	uVec = abs(inVec.x) < 0.999 ? vec3(1,0,0) : vec3(0,1,0);
	uVec = normalize(cross(inVec, uVec));
	vVec = normalize(cross(inVec, uVec));
}

void main() {
	worldSpacePos = (m * vec4(pos, 1.0)).xyz;
	worldSpaceNormal = (n * vec4(normalize(normal), 1.0)).xyz;
	UV = texCoord;

	computeTangentVectors(worldSpaceNormal, worldSpaceTangent, worldSpaceBitangent);

    gl_Position = p * v * vec4(worldSpacePos, 1.0);
}