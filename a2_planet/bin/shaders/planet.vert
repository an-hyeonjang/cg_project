#version 330

in vec3 position;	
in vec3 normal;
in vec2 texcoord;

out vec3 norm;	
out vec2 tc;	
out vec4 epos;	// eye-space position

uniform mat4	model_matrix;	// 4x4 transformation matrix: explained later in the lecture
uniform mat4	aspect_matrix;	// tricky 4x4 aspect-correction matrix
uniform mat4	view_projection_matrix;

void main()
{
	gl_Position = aspect_matrix*view_projection_matrix*model_matrix*vec4(position,1);
	norm = normalize(mat3(view_projection_matrix*model_matrix)*normal);
	tc = texcoord;
}
