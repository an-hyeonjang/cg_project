#version 330

// input attributes of vertices
in vec3 position;	
in vec3 normal;
in vec2 texcoord;

out vec3 norm;
out vec2 tc;
out vec4 epos;

// uniform variables
uniform mat4	model_matrix;
uniform mat4	aspect_matrix;

void main()
{
	gl_Position = aspect_matrix * model_matrix * vec4(position, 1);

	norm = normalize(mat3(aspect_matrix * model_matrix)*normal);
	tc = texcoord;
}