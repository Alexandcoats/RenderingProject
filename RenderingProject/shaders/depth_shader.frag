#version 450

uniform int octant;

void main() {
	// Write to the depth buffer
	// Discard any fragments outside of the octant
	vec2 screenspaceCoord = gl_FragCoord.xy / 256.0; // Get the screen space coord in [0, 1]
	if		((octant == 0 || octant == 5) && screenspaceCoord.y >= screenspaceCoord.x) discard;
	else if ((octant == 1 || octant == 4) && screenspaceCoord.y < screenspaceCoord.x) discard;
	else if ((octant == 2 || octant == 7) && screenspaceCoord.y + screenspaceCoord.x >= 1.0) discard;
	else if ((octant == 3 || octant == 6) && screenspaceCoord.y + screenspaceCoord.x < 1.0) discard;
}