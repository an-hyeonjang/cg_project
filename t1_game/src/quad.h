#pragma once
#ifndef __QUAD_H_
#define __QUAD_H_

struct quad_t
{
	GLuint	vertex_buffer;
	vertex	vertices[4];
	vec2	center = vec2(0);		// 2D position for translation
	float	scale = 1.0f;		// radius
	mat4	model_matrix;		// modeling transformation
	

	quad_t() 
	{
		vertices[0] = { vec3(-1,-1,0),vec3(0,0,1),vec2(0,0) };
		vertices[1] = { vec3(1,-1,0),vec3(0,0,1),vec2(1,0) };
		vertices[2] = { vec3(-1,1,0),vec3(0,0,1),vec2(0,1) };
		vertices[3] = { vec3(1,1,0),vec3(0,0,1),vec2(1,1) };

		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &vertices[0], GL_STATIC_DRAW);
	}
};

void update_quad_vertex(vertex* vertices)
{
	vertex corners[4];
	corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);

	vertex v[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };
	vertices = v;
}

std::vector<vertex> update_quad_vertex(std::vector<vertex> vertices)
{
	vertex corners[4];
	corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	
	vertices = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	return vertices;
}

//void update_quad_vertex()
//{
//	static vertex vertices[] = 
//	{ 
//		{vec3(-1,-1,0),vec3(0,0,1),vec2(0,0)}, 
//		{vec3(1,-1,0),vec3(0,0,1),vec2(1,0)}, 
//		{vec3(-1,1,0),vec3(0,0,1),vec2(0,1)}, 
//		{vec3(1,1,0),vec3(0,0,1),vec2(1,1)} 
//	}; // strip ordering [0, 1, 3, 2]
//
//	glGenBuffers(1, &vertex_buffer);
//	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * 4, &vertices[0], GL_STATIC_DRAW);
//}

void draw_quad(GLuint program, GLuint vertex_buffer, GLuint texture)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindTexture(GL_TEXTURE_2D, texture);
	
	cg_bind_vertex_attributes(program);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif