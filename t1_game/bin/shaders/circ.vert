#version 330

// input attributes of vertices
in vec3 position;	
in vec3 normal;
in vec2 texcoord;

out vec3 norm;
out vec2 tc;
out vec4 epos;

// uniform variables
uniform mat4	model_matrix;	// 4x4 transformation matrix: explained later in the lecture
uniform mat4	view_matrix;
uniform mat4	projection_matrix;

uniform int		pass_n;

void main()
{
	vec4 wpos = model_matrix * vec4(position,1);
	epos = view_matrix * wpos;
	if(pass_n == 1) gl_Position = projection_matrix * epos;
	if(pass_n == 2) gl_Position = vec4(position, 1);

	norm = normalize(mat3(view_matrix*model_matrix)*normal);
	tc = texcoord;
}