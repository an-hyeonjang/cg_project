#pragma once
#ifndef __QUAD_H_
#define __QUAD_H_

struct quad_t
{
	GLuint	VAO;
	GLuint	vertex_buffer;
	vertex	vertices[4];
	vec2	center = vec2(0);		// 2D position for translation

	void init()
	{
		//glGenVertexArrays(1, &VAO);
		//glBindVertexArray(VAO);

		vertices[0] = { vec3(-1,-1,0),vec3(0,0,1),vec2(0,0) };
		vertices[1] = { vec3(1,-1,0),vec3(0,0,1),vec2(1,0) };
		vertices[2] = { vec3(-1,1,0),vec3(0,0,1),vec2(0,1) };
		vertices[3] = { vec3(1,1,0),vec3(0,0,1),vec2(1,1) };

		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &vertices[0], GL_STATIC_DRAW);
		//cg_bind_vertex_attributes(program);
		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	};

	void render(GLuint program, GLuint vertex_buffer, GLuint texture)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		cg_bind_vertex_attributes(program);

		glBindTexture(GL_TEXTURE_2D, texture);
	
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
};

struct brick_t
{
	vec2	center;
	float	scale = 1;
	mat4	model_matrix;

	brick_t(vec2 c, float f) { center = c; scale = f; };

	void update()
	{
		model_matrix = mat4::translate(vec3(center, 0)) * mat4::scale(scale);
	};
};

#endif