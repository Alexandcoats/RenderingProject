#version 450

uniform int octant;

void main() {
	// Write to the depth buffer
	// Discard any fragments outside of the octant
	if		((octant == 0 || octant == 5) && gl_FragCoord.y > gl_FragCoord.x) discard;
	else if ((octant == 1 || octant == 4) && gl_FragCoord.y < gl_FragCoord.x) discard;
	else if ((octant == 2 || octant == 7) && gl_FragCoord.y > -gl_FragCoord.x) discard;
	else if ((octant == 3 || octant == 6) && gl_FragCoord.y < -gl_FragCoord.x) discard;
}