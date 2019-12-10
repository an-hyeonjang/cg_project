#version 330

// vertex attributes
in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec2 tc;	// texture coordinate

// uniforms

uniform mat4 model_matrix;
uniform mat4 aspect_matrix;

void main()
{
	gl_Position = model_matrix * vec4(position, 1);
	tc = texcoord;
}