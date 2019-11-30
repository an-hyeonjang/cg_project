#version 330

in vec3 norm;
in vec2 tc;	

out vec4 fragColor;

uniform bool tex_flag;

uniform sampler2D TEX;

void main()
{
	fragColor = texture(TEX, tc);
}

