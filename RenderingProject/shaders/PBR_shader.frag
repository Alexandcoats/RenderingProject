#version 450

uniform sampler2D texSampler;
uniform vec3 camPos;
uniform vec3 lightPos;

// PBR inputs
uniform vec3 baseColor;
uniform float roughness;
uniform float specular;
uniform float clearcoat;

in vec3 worldPos;
in vec3 worldNormal;
in vec2 UV;

out vec4 outColor;

const float PI = 3.14159265358979323846;

float l1Norm(vec3 v) {
	return abs(v.x) + abs(v.y) + abs(v.z);
}

float angle(vec3 v1, vec3 v2) {
	return acos(dot(v1, v2) / (l1Norm(v1) * l1Norm(v2)));
}

vec3 diffuse(float theta_d, float theta_l, float theta_v) {
	float F_D90 = 0.5 + (2 * cos(theta_d*theta_d) * roughness);
	vec3 f_d = (baseColor / PI) * (1 + (F_D90 - 1) * pow(1-cos(theta_l), 5)) * (1 + (F_D90 - 1) * pow(1 - cos(theta_v), 5))
	return f_d;
}

vec3 D(float theta_h) {
	float a = roughness * roughness;
	float c_spec = specular * 0.08;
	float c_cc = clearcoat * 0.25;
	vec3 d_base = c_spec / pow(a*a*pow(cos(theta_h),2) + pow(sin(theta_h),2), 2);
	vec3 d_cc = c_cc / (a*a*pow(cos(theta_h),2) + pow(sin(theta_h),2));

	return d_base + d_cc;
}

float F(float theta_d) {
	return 
}

void main() {
	//outColor = blinnphong();

	vec3 viewDir = normalize(camPos - worldPos);
	vec3 lightDir = normalize(lightPos - worldPos);
	vec3 halfway = normalize(lightDir + viewDir);
	float theta_d = angle(lightDir, halfway);
	float theta_l = angle(lightDir, normalize(worldNormal))
	float theta_v = angle(viewDir, normalize(worldNormal))
	float theta_h = angle(halfway, normalize(worldNormal))

	vec3 res = diffuse(theta_d, theta_l, theta_v) + ((D() * F() * G())/(4 * cos(theta_l) * cos(theta_v)));
}