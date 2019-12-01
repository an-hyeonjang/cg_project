#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

const uint NUM_TESS = 72;

struct circle_t
{
	GLuint	vertex_buffer, index_buffer;
	std::vector<vertex>	vertices;
	
	uint	index;				// index
	vec2	position=vec2(0);		// 2D position for translation
	float	size=1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	vec2	velocity = vec2(0.1f);
	float	mass = size*size;

	mat4	model_matrix;		// modeling transformation
	void	update( float t ); 
	float	speed_scale = 150.0f;

	void init()
	{
		//std::vector<vertex> v = { { vec3(0), vec3(0), vec2(0) } };
		//for (uint k = 0; k <= NUM_TESS; k++)
		//{
		//	float t = PI * 2.0f * k / float(NUM_TESS), c = cos(t), s = sin(t);
		//	v.push_back({ vec3(c,s,0), vec3(c,s,c*s), vec2(c,s) });
		//}

		//for (uint k = 0; k < NUM_TESS; k++)
		//{
		//	vertices.push_back(v.front());	// the origin
		//	vertices.push_back(v[k + 1]);
		//	vertices.push_back(v[k + 2]);
		//}

		//// generation of vertex buffer: use triangle_vertices instead of vertices
		//glGenBuffers(1, &vertex_buffer);
		//glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		uint N = NUM_TESS;

		std::vector<vertex> v;
		for (uint k = 0; k <= N; k++)
		{
			float lngi = PI * 2.0f * k / float(N), lns = sin(lngi), lnc = cos(lngi);
			for (uint i = 0; i <= N / 2; i++)
			{
				float lati = PI * 1.0f * i / float(N / 2), lac = cos(lati), las = sin(lati);
				vertices.push_back({ vec3(las * lnc, las * lns ,lac), vec3(las * lnc, las * lns ,lac), vec2(lngi / (2 * PI), 1.0f - lati / PI) });
			}
		}

		std::vector<uint> indices;
		for (uint k = 0; k < N + 2; k++)
		{
			uint n = N / 2;
			for (uint i = 0; i < n; i++)
			{
				indices.push_back(k * n + i);
				indices.push_back(k * n + i + 1);
				indices.push_back((k + 1) * n + i + 1);
				indices.push_back(k * n + i + 1);
				indices.push_back((k + 1) * n + i + 2);
				indices.push_back((k + 1) * n + i + 1);
			}
		}

		// generation of vertex buffer: use vertices as it is
		glGenBuffers(1, &vertex_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// geneation of index buffer
		glGenBuffers(1, &index_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), &indices[0], GL_STATIC_DRAW);
	}
};


inline void circle_t::update(float t)
{
	position += velocity/speed_scale;

	mat4 scale_matrix = mat4::scale(size);
	mat4 rotation_matrix = mat4();
	mat4 translate_matrix = mat4::translate(position.x, position.y, 0);
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}


#endif
