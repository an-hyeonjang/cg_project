#version 330

// input attributes of vertices
in vec3 position;	
in vec3 normal;
in vec2 texcoord;

// outputs of vertex shader = input to fragment shader
// out vec4 gl_Position: a built-in output variable that should be written in main()
out vec3 norm;	// the second output: not used yet
out vec2 tc;	// the third output: not used yet

// uniform variables
uniform mat4	model_matrix;	// 4x4 transformation matrix: explained later in the lecture
uniform mat4	aspect_matrix;	// tricky 4x4 aspect-correction matrix

void main()
{
	gl_Position = aspect_matrix*model_matrix*vec4(position,1);

	// another output passed via varying variable
	norm = normal;
	tc = texcoord;
}