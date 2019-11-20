#pragma once
#ifndef __CIRCLE_H_
#define __CIRCLE_H_


std::vector<vertex> create_circle_vertices(uint N)
{
	std::vector<vertex> v = { { vec3(0), vec3(0,0,-1.0f), vec2(0.5f) } }; // origin
	for (uint k = 0; k <= N; k++)
	{
		float t = PI * 2.0f * k / float(N), c = cos(t), s = sin(t);
		v.push_back({ vec3(c,s,0), vec3(0,0,-1.0f), vec2(c,s)});
	}
	return v;
}

void draw_circle(GLuint program, GLuint vertex_buffer)
{
	uint point_n = 72;
	GLuint index_buffer = 0;
	std::vector<vertex> vertices = create_circle_vertices(point_n);

	// check exceptions
	if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

	std::vector<uint> indices;
	for (uint k = 0; k < point_n; k++)
	{
		indices.push_back(0);	// the origin
		indices.push_back(k + 1);
		indices.push_back(k + 2);
	}

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);

	cg_bind_vertex_attributes(program);
	glDrawElements(GL_TRIANGLES, point_n * 3, GL_UNSIGNED_INT, nullptr);	
}

#endif