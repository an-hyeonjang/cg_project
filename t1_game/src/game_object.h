#pragma once
#ifndef __GAME_OBJECT_H_
#define	__GAME_OBJECT_H_

#include <iostream>

#include "texture.h"
#include "quad.h"

GLuint program_object;
GLuint VAO;
quad_t	quad;

const char* sprites_vert_shader = "../bin/shaders/cg.vert";
const char* sprites_frag_shader = "../bin/shaders/cg.frag";

mat4 aspect_matrix;

void game_object_init()
{
	if (!(program_object = cg_create_program(sprites_vert_shader, sprites_frag_shader))) { printf("Loading map is failed\n"); glfwTerminate(); return; }
	quad.init();
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
}

void game_update(ivec2 window_size)
{
	float aspect = window_size.x / (float)window_size.y;
	aspect_matrix =
	{
		min(1 / aspect,1.0f), 0, 0, 0,
		0, min(aspect,1.0f), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
}


struct brick_t
{
	vec2	center;
	float	scale = 1;
	mat4	model_matrix;

	brick_t(vec2 c, float f) { center = c; scale = f; };

	void update()
	{
		model_matrix = mat4::translate(vec3(center,0)) * mat4::scale(scale);
	};
};

struct map_t
{
	std::vector<brick_t> bricks;
	Texture tex;
	vec2	size = vec2(0.5f);

	ivec2 w_size;

	std::vector<std::vector<GLuint>> tileData = 
	{
		{1,1,1,1,1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{1,1,1,1,1,1,1,1,1,1,1,1}
	};
	
	void init(ivec2 window_size)
	{
		tex.load("../bin/images/Snowflake.png");
		w_size = window_size;

		uint height = tileData.size();
		uint width = tileData[0].size();
		vec2 unit_size = vec2( size.x / (float)width, size.y / (float)height );
		for (uint y = 0; y < height; y++)
		{
			for (uint x = 0; x < width; x++)
			{
				// Check block type from level data (2D level array)
				if (tileData[y][x] == 1) // Solid
				{
					vec2 pos = size * vec2( (float)x , (float)y ) - vec2(size.x*height/2, size.y*width/2);
					brick_t b = { pos, 0.2f };
					bricks.push_back(b);
				}
			}
		}
	};

	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(program_object);

		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);

		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		cg_bind_vertex_attributes(program_object);

		for (auto& b : bricks)
		{
			b.update();
			glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, b.model_matrix);
			glBindTexture(GL_TEXTURE_2D, tex.texture[0]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	};
};

struct creature
{
	//model and images;
	quad_t	model = quad_t();
	Texture	tex;

	//attribute for games;
	uint	life;

	//control function
	void update();
};

struct player_t
{
	Texture	tex;
	float	scale = 0.3f;
	uint	mov_i;

	//model_matrix
	ivec2 w_size;
	mat4 model_matrix;
	GLuint texture;

	//attribute for games;
	uint	life;

	player_t(float size = 1.0f):scale(size) { model_matrix = mat4::scale(size); }

	//image_path
	std::vector<const char*> image_path;
	

	void init(ivec2 window_size)
	{
		if (!(program_object = cg_create_program(sprites_vert_shader, sprites_frag_shader))) { printf("Loading map is failed\n"); glfwTerminate(); return; }
		tex.load("../bin/images/sprites/mech-unit-export1.png");
		tex.load("../bin/images/sprites/mech-unit-export2.png");
		tex.load("../bin/images/sprites/mech-unit-export3.png");
		tex.load("../bin/images/sprites/mech-unit-export4.png");
		tex.load("../bin/images/sprites/mech-unit-export5.png");
		tex.load("../bin/images/sprites/mech-unit-export6.png");
		tex.load("../bin/images/sprites/mech-unit-export7.png");
		tex.load("../bin/images/sprites/mech-unit-export8.png");

		mov_i = 0;

		w_size = window_size;
	};

	//control function
	void moving(vec3 move)
	{
		mov_i++;
		mov_i = mov_i % tex.texture.size();
		model_matrix *= mat4::translate(move);
	};

	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program_object);

		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);

		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		
		cg_bind_vertex_attributes(program_object);

		glBindTexture(GL_TEXTURE_2D, tex.texture[mov_i]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	};
};


#endif