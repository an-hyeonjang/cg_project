#version 330

// input attributes of vertices
in vec3 position;	
in vec3 normal;
in vec2 texcoord;

out vec3 norm;
out vec2 tc;

uniform mat4	model_matrix;	// 4x4 transformation matrix: explained later in the lecture
uniform mat4	aspect_matrix;

void main()
{
	gl_Position = aspect_matrix * model_matrix *vec4(position, 1);
	tc = texcoord;
}