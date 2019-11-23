#pragma once
#ifndef __QUAD_H_
#define __QUAD_H_

struct quad_t
{
	vec2	center = vec2(0);		// 2D position for translation
	float	scale = 1.0f;		// radius
	mat4	model_matrix;		// modeling transformation
	// public functions
	void	update(float t);
};

inline std::vector<quad_t> create_quads()
{
	std::vector<quad_t> quad;

	quad_t q = { vec2(0), 0.5f };

	quad.push_back(q);

	return quad;
}

inline void quad_t::update(float t)
{
	mat4 scale_matrix = mat4::scale(scale);
	mat4 rotation_matrix = mat4::rotate(vec3(0, 0, 1), 0);
	mat4 translate_matrix = mat4::translate(0);

	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
}

void draw_quad(GLuint program, GLuint vertex_buffer, GLuint texture)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vertex corners[4];
	corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
	corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
	corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
	corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
	vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindTexture(GL_TEXTURE_2D, texture);

	cg_bind_vertex_attributes(program);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}
#endif