#version 330

in vec3 norm;
in vec2 tc;	

out vec4 fragColor;

uniform sampler2D TEX;

void main()
{
	fragColor = vec4(norm, 1);
}

