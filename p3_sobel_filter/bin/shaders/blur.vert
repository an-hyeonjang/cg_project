#version 330

// vertex attributes
in vec3 position;
in vec3 normal;
in vec2 texcoord;


uniform int pass_n;

out vec2 tc;	// texture coordinate

void main()
{
	if(pass_n == 1) gl_Position = vec4(position, 1);
	if(pass_n == 2) gl_Position = vec4(position,1);
	tc = texcoord;
}