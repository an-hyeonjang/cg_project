#pragma once
#ifndef __GAME_OBJECT_H_
#define	__GAME_OBJECT_H_

#include "texture.h"
#include "quad.h"
#include "circle.h"
#include "wall.h"
#include "light.h"

#include "irrKlang\irrKlang.h"
#pragma comment(lib, "irrKlang.lib" )

GLuint program_object, program_obj_color;

quad_t		quad;
circle_t	circle;
wall_t		wall;
light_t		light;
material_t	material;

float z_depth = 0.6f;

const char* sprites_vert_shader = "../bin/shaders/cg.vert";
const char* sprites_frag_shader = "../bin/shaders/cg.frag";
const char* color_vert_shader = "../bin/shaders/circ.vert";
const char* color_frag_shader = "../bin/shaders/circ.frag";

mat4 aspect_matrix;

//*********************************************************
//irrklang

irrklang::ISoundEngine* engine;
irrklang::ISoundSource* back_src = nullptr;
irrklang::ISoundSource* bubble_effect_src = nullptr;
irrklang::ISoundSource* attack_effect_src = nullptr;

const char* effect_src_a = "../bin/sounds/attack.wav";
const char* effect_src_b = "../bin/sounds/bubble.wav";

enum object_state
{
	wait,
	moving,
	hit_a,
	hit_b,
	hit_c,
	dead
};

void game_object_init(ivec2 window_size)
{
	if (!(program_object = cg_create_program(sprites_vert_shader, sprites_frag_shader))) { printf("Loading map is failed\n"); glfwTerminate(); return; }
	if (!(program_obj_color = cg_create_program(color_vert_shader, color_frag_shader))) { printf("Loading map is failed\n"); glfwTerminate(); return; }

	quad.init();
	circle.init();
	wall.init(window_size);

	//irrklang
	engine = irrklang::createIrrKlangDevice();
	if (!engine) return;

	//add sound source from the sound file
	//back_src = engine->addSoundSourceFromFile(src1);
	attack_effect_src = engine->addSoundSourceFromFile(effect_src_a);
	bubble_effect_src = engine->addSoundSourceFromFile(effect_src_b);

	attack_effect_src->setDefaultVolume(0.4f);
	bubble_effect_src->setDefaultVolume(1.2f);
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
		tex.load_mip("../bin/images/back.png");
	}

	void render()
	{
		//glDisable(GL_DEPTH_TEST);

		glUseProgram(program_object);

		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		cg_bind_vertex_attributes(program_object);

		mat4 aspect_matrix = mat4();
		mat4 model_matrix = mat4::translate(vec3(0,0,z_depth+0.3f)) * mat4::scale(1.0f);

		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);

		glBindTexture(GL_TEXTURE_2D, tex.texture[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//glEnable(GL_DEPTH_TEST);
	};

};

struct creature_t
{
	Texture	tex;
	float	scale;

	//model_matrix
	mat4 model_matrix;
	uint	mov_i;

	//attribute for games;
	uint	index;
	vec3	position = vec3(0);
	vec2	size;

	uint	hit_on;
	uint	life;
	vec3	velocity = vec3(0.1f);
	float	mass = 1.0f;

	void init(float scale)
	{	
		for (auto& t : tex.texture)
			if (t) glDeleteTextures(1, &t);
		tex.load("../bin/images/creature/1.png");

		size = scale * vec2(1.0f, 1.0f);
		mov_i = 0;
	};

	void update(float t)
	{
		position += velocity / 100.0f;
		position = vec3(position.x, position.y, min(position.z, z_depth));
		model_matrix = mat4::translate(position) * mat4::scale(size.x, size.y, 0);
	}

	void render_quad()
	{
		float t = (float)glfwGetTime();

		glUseProgram(program_object);
		 
		update(t);

		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);

		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		cg_bind_vertex_attributes(program_object);

		glBindTexture(GL_TEXTURE_2D, tex.texture[mov_i]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindTexture(GL_TEXTURE_2D, 0);
	};

	void render_circle()
	{
		float t = (float)glfwGetTime();

		glUseProgram(program_obj_color);

		vec3	eye = vec3(0.0f, 0.0f, -5.0f);
		vec3	at = vec3(0);
		vec3	up = vec3(0, 1.0f, 0);

		model_matrix = mat4::translate(position) * mat4::scale(size.x, size.y, 0);
		mat4 view_matrix = mat4::rotate(vec3(1, 0, 0), PI / 2) *  mat4::look_at(eye, at, up);

		glUniform1f(glGetUniformLocation(program_obj_color, "time"), t);

		glUniform4fv(glGetUniformLocation(program_obj_color, "light_position"), 1, light.position);
		glUniform4fv(glGetUniformLocation(program_obj_color, "Ia"), 1, light.ambient);
		glUniform4fv(glGetUniformLocation(program_obj_color, "Id"), 1, light.diffuse);
		glUniform4fv(glGetUniformLocation(program_obj_color, "Is"), 1, light.specular);

		// setup material properties
		glUniform4fv(glGetUniformLocation(program_obj_color, "Ka"), 1, material.ambient);
		glUniform4fv(glGetUniformLocation(program_obj_color, "Kd"), 1, material.diffuse);
		glUniform4fv(glGetUniformLocation(program_obj_color, "Ks"), 1, material.specular);
		glUniform1f(glGetUniformLocation(program_obj_color, "shininess"), material.shininess);

		glUniformMatrix4fv(glGetUniformLocation(program_obj_color, "view_matrix"), 1, GL_TRUE, view_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_obj_color, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_obj_color, "model_matrix"), 1, GL_TRUE, model_matrix);

		glBindBuffer(GL_ARRAY_BUFFER, circle.vertex_buffer);
		cg_bind_vertex_attributes(program_obj_color);
		  
		//glBindTexture(GL_TEXTURE_2D, tex.texture[0]);
		glDrawElements(GL_TRIANGLES, (NUM_TESS + 1) * (NUM_TESS + 1) * 3, GL_UNSIGNED_INT, nullptr);

		glBindTexture(GL_TEXTURE_2D, 0);
	};

};


struct player_t
{
	//draw attribute

	Texture	normal;
	Texture walk;
	Texture	attack;

	float	scale = 0.3f;
	float	timer;

	//model_matrix
	mat4 model_matrix;
	GLuint texture;

	//game attribute for games;
	vec3	position = vec3(-1.0f, 0.0f,0.0f);
	vec3	result = vec3(0);
	vec3	size = vec3(scale, scale, 0);

	uint	state;
	uint	life = 3;
	uint	hit_on;
	uint	mov_i;
	uint	mov;
	
	void init()
	{
		for (auto& t : normal.texture)
			if (t) glDeleteTextures(1, &t);

		normal.load("../bin/images/sprites/1.png");
		normal.load("../bin/images/sprites/2.png");
		normal.load("../bin/images/sprites/3.png");

		for (auto& t : walk.texture)
			if (t) glDeleteTextures(1, &t);

		walk.load("../bin/images/sprites/walk1.png");
		walk.load("../bin/images/sprites/walk2.png");
		walk.load("../bin/images/sprites/walk3.png");
		walk.load("../bin/images/sprites/walk4.png");
		walk.load("../bin/images/sprites/walk5.png");
		walk.load("../bin/images/sprites/walk6.png");
		walk.load("../bin/images/sprites/walk7.png");

		for (auto& t : attack.texture)
			if (t) glDeleteTextures(1, &t);

		attack.load("../bin/images/sprites/punch1.png");
		attack.load("../bin/images/sprites/punch2.png");
		attack.load("../bin/images/sprites/punch3.png");

		mov_i = 0;
		mov = 0;
		hit_on = 0;
		model_matrix = mat4::translate(position) * mat4::scale(scale, scale*1.3f, scale);

		timer = (float)glfwGetTime();
	};

	void update()
	{
		size = vec3(scale, scale, scale);
		if ((float)glfwGetTime() > timer) { state = wait; timer += 4.0f;  printf("%f, %f\n", timer, (float)glfwGetTime()); }
		model_matrix = mat4::translate(position) * mat4::scale(scale, scale * 1.3f, scale);
	}

	//control function
	void render(float t)
	{
		int n = (int)(sin(t)*sin(t)*normal.texture.size());

		glUseProgram(program_object);
		
		update();

		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		cg_bind_vertex_attributes(program_object);
		
		if (state == hit_a)
		{
			size = vec3(scale * 1.2f, scale * 1.3f, scale);
			model_matrix = mat4::translate(position) * mat4::scale(size);
			glBindTexture(GL_TEXTURE_2D, attack.texture[2]);
		}
		else if (state == hit_b) glBindTexture(GL_TEXTURE_2D, attack.texture[1]);
		else if(state == wait) glBindTexture(GL_TEXTURE_2D, normal.texture[n]);
		else if (state == moving)	glBindTexture(GL_TEXTURE_2D, walk.texture[mov]);
		
		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
		glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	};

	void control_axis(int key, int action)
	{
		vec3 xyz_move = (0.04f);	

		mov_i++;
		mov = (int)floor(mov_i/2) % walk.texture.size();

		if (position.y + 0.20f > z_depth || position.z + 0.20f > z_depth) position -= vec3(0,xyz_move.y, xyz_move.z);

		if (key == GLFW_KEY_RIGHT)
		{
			state = moving;

			position += vec3(xyz_move.x, 0, 0);
			if (key == GLFW_KEY_UP) position += vec3(xyz_move);
			else if (key == GLFW_KEY_DOWN) position -= vec3(xyz_move);
		}
		if (key == GLFW_KEY_LEFT)
		{
			state = moving;

			position -= vec3(xyz_move.x, 0, 0);
			if (key == GLFW_KEY_UP) position += vec3(xyz_move);
			else if (key == GLFW_KEY_DOWN) position -= vec3(xyz_move);
		}
		if (key == GLFW_KEY_UP)
		{
			state = moving;

			position += vec3(0, xyz_move.y, xyz_move.z);
			if (key == GLFW_KEY_RIGHT) position += vec3(xyz_move.x, xyz_move.y, 0);
			else if (key == GLFW_KEY_LEFT) position -= vec3(xyz_move.x, xyz_move.y, 0);
		}
		if (key == GLFW_KEY_DOWN)
		{
			state = moving;

			position -= vec3(0, xyz_move.y, xyz_move.z);
			if (key == GLFW_KEY_RIGHT) position += vec3(xyz_move.x, xyz_move.y, 0);
			else if (key == GLFW_KEY_LEFT) position -= vec3(xyz_move.x, xyz_move.y, 0);
		}

	};

	void control(int key, int action)
	{
		int previous_state = wait;
		if (key == GLFW_KEY_Z)
		{
			if (action == GLFW_PRESS)
			{
				engine->play2D(attack_effect_src, false);
				state = hit_a;
				this->hit_on = 1;
			}
			else if (action == GLFW_RELEASE)
			{
				state = hit_b;
				this->hit_on = 0;
			}
		}
	}
};


//struct map_t
//{
//	std::vector<brick_t> bricks;
//	Texture tex;
//	vec2	size = vec2(0.5f);
//
//	std::vector<std::vector<GLuint>> tileData =
//	{
//		{1,1,1,1,1,1,1,1,1,1,1,1},
//		{0,0,0,0,0,0,0,0,0,0,0,0},
//		{1,1,1,1,1,1,1,1,1,1,1,1},
//		{0,0,0,0,0,0,0,0,0,0,0,0},
//		{1,1,1,1,1,1,1,1,1,1,1,1},
//		{0,0,0,0,0,0,0,0,0,0,0,0},
//		{1,1,1,1,1,1,1,1,1,1,1,1}
//	};
//
//	void init()
//	{
//		if (tex.texture[0]) glDeleteTextures(1, &tex.texture[0]);
//		tex.load("../bin/images/creature/1.png");
//
//		uint height = tileData.size();
//		uint width = tileData[0].size();
//		vec2 unit_size = vec2(size.x / (float)width, size.y / (float)height);
//		for (uint y = 0; y < height; y++)
//		{
//			for (uint x = 0; x < width; x++)
//			{
//				// Check block type from level data (2D level array)
//				if (tileData[y][x] == 1) // Solid
//				{
//					vec2 pos = size * vec2((float)x, (float)y) - vec2(size.x * height / 2, size.y * width / 2);
//					brick_t b = { pos, 0.2f };
//					bricks.push_back(b);
//				}
//			}
//		}
//	};
//
//	void render()
//	{
//
//		glUseProgram(program_object);
//
//		glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
//
//		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
//		cg_bind_vertex_attributes(program_object);
//
//		for (auto& b : bricks)
//		{
//			b.update();
//			glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, b.model_matrix);
//			glBindTexture(GL_TEXTURE_2D, tex.texture[0]);
//			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//		}
//	};
//};

#endif