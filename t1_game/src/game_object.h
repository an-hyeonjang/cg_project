#pragma once
#ifndef __GAME_OBJECT_H_
#define	__GAME_OBJECT_H_

#include <iostream>

#include "texture.h"
#include "quad.h"

GLuint program_object;
quad_t	quad;

const char* sprites_vert_shader = "../bin/shaders/cg.vert";
const char* sprites_frag_shader = "../bin/shaders/cg.frag";

mat4 aspect_matrix;

void game_object_init()
{
	if (!(program_object = cg_create_program(sprites_vert_shader, sprites_frag_shader))) { printf("Loading map is failed\n"); glfwTerminate(); return; }
	quad.init();

	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
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

struct background_t
{
	Texture tex;

	void init()
	{
		tex.load("../bin/images/back.png");
	}

	void render()
	{
		glDisable(GL_DEPTH_TEST);

		glUseProgram(program_object);

		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		cg_bind_vertex_attributes(program_object);

		mat4 aspect_matrix = mat4();
		mat4 model_matrix = mat4::scale(1.5f);

		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);

		glBindTexture(GL_TEXTURE_2D, tex.texture[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glEnable(GL_DEPTH_TEST);
	};

};


struct map_t
{
	std::vector<brick_t> bricks;
	Texture tex;
	vec2	size = vec2(0.5f);

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
	
	void init()
	{
		if (tex.texture[0]) glDeleteTextures(1, &tex.texture[0]);
		tex.load("../bin/images/Snowflake.png");

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

struct creature_t
{
	Texture	tex;
	float	scale = 0.1f;

	//model_matrix
	mat4 model_matrix;
	uint	mov_i;

	//attribute for games;
	vec3	position = vec3(0);
	vec3	size = vec3(scale, scale, 0);

	uint	hit_on;
	uint	life;

	creature_t(float size = 1.0f) :scale(size) { model_matrix = mat4::scale(size); }

	void init()
	{	
		for (auto& t : tex.texture)
			if (t) glDeleteTextures(1, &t);
		tex.load("../bin/images/Snowflake.png");

		mov_i = 0;
	};

	//control function

	void update()
	{
		model_matrix = mat4::translate(position) * mat4::scale(scale);
	}

	void render()
	{
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program_object);
		 
		this->update();

		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);

		//glBindVertexArray(quad.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		cg_bind_vertex_attributes(program_object);

		glBindTexture(GL_TEXTURE_2D, tex.texture[mov_i]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	};

	void moving(vec3 move)
	{
		mov_i++;
		mov_i = mov_i % tex.texture.size();
		model_matrix *= mat4::translate(move);
	};


};
  
struct player_t
{
	//draw attribute

	Texture	normal;
	Texture walk;
	Texture	attack;

	float	scale = 0.3f;
	uint	mov_i;

	//model_matrix
	mat4 model_matrix;
	GLuint texture;

	//game attribute for games;
	vec3	position = vec3(0);
	vec3	size = vec3(scale, scale, 0);

	uint	state;
	uint	life = 3;
	uint	hit_on;
	
	void init()
	{
		for (auto& t : normal.texture)
			if (t) glDeleteTextures(1, &t);

		normal.load("../bin/images/sprites/1.png");
		normal.load("../bin/images/sprites/2.png");

		for (auto& t : walk.texture)
			if (t) glDeleteTextures(1, &t);

		walk.load("../bin/images/sprites/walk1.png");
		walk.load("../bin/images/sprites/walk2.png");
		walk.load("../bin/images/sprites/walk3.png");
		walk.load("../bin/images/sprites/walk4.png");
		walk.load("../bin/images/sprites/walk5.png");
		walk.load("../bin/images/sprites/walk6.png");
		walk.load("../bin/images/sprites/walk7.png");
		walk.load("../bin/images/sprites/walk8.png");

		for (auto& t : attack.texture)
			if (t) glDeleteTextures(1, &t);

		attack.load("../bin/images/sprites/punch1.png");
		attack.load("../bin/images/sprites/punch2.png");
		attack.load("../bin/images/sprites/punch3.png");

		mov_i = 0;
		hit_on = 0;
		model_matrix = mat4::translate(position) * mat4::scale(scale);
	};

	void update()
	{
		model_matrix = mat4::translate(position) * mat4::scale(scale);
	}

	//control function
	void render(float t)
	{
		int k = (int)(t*3);
		glUseProgram(program_object);
		
		this->update();

		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);

		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		cg_bind_vertex_attributes(program_object);
	
		if(hit_on)
			glBindTexture(GL_TEXTURE_2D, attack.texture[2]);
		else if (!mov_i) glBindTexture(GL_TEXTURE_2D, normal.texture[0]);
		else glBindTexture(GL_TEXTURE_2D, walk.texture[mov_i]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	};

	void moving(vec3 move)
	{
		mov_i++;
		mov_i = mov_i % walk.texture.size();

		position += move;
	};

	void control(int key, int action)
	{
		if (key == GLFW_KEY_Z)
		{
			if (action == GLFW_PRESS)
				this->hit_on = 1;
			else if(action == GLFW_RELEASE)
				this->hit_on = 0;
		}
	}

};




#endif