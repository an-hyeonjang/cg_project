#version 330

in vec4 epos;	// eye-space position
in vec3 norm;	// per-vertex normal before interpolation
in vec2 tc;	// texture coordinate

uniform float t;

out vec4 fragColor;

void main()
{
	fragColor = vec4(tc.xy*t, 1.0, 1.0);
}