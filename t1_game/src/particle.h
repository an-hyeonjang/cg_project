#pragma once
#ifndef __PARTICLE_H__
#define __PARTICLE_H_

#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

#include "quad.h"
#include "texture.h"

//*************************************
// global constants
static const char* particle_v_shader_path = "../bin/shaders/particle.vert";
static const char* particle_f_shader_path = "../bin/shaders/particle.frag";

//*************************************
// opengl attributes
GLuint	program_particle = 0;	// ID holder for GPU program
GLuint	TEX = 0;

//*************************************
//Random Func
inline const float random_range(float min, float max) { return min + rand() / (RAND_MAX / (max - min)); }
const vec3 gravity = vec3(0, 9.8f, 0);

//*************************************
// common structures
struct particle_t
{
	vec3 position = vec3(0);
	vec3 pos;
	vec4 color;
	vec3 velocity;
	float scale;
	float life;

	float radius, height = 4 * length(gravity);
	float theta;

	float camera_dis;

	float current_time = 0;
	float previous_time = (float)glfwGetTime();

	mat4 model_matrix = mat4::translate(pos) * mat4::scale(scale);

	particle_t() { reset(); }

	void reset()
	{
		float r = 1.0f + float(rand() % 256) / 255.0f;
		float g = float(rand() % 256) / 255.0f;
		float b = 0;
		float w = 1.0f;

		pos = vec3(position);

		life = random_range(0.1f, 1.0f);

		color = vec4(r, g, b, w);
		scale = random_range(0.02f, 0.04f);

		theta = float(rand() % 360 * PI) / 180.0f;
		radius = float(rand() % 100) / 20.0f;

		vec3 main_direction = vec3(radius * cos(theta), random_range(height - 2.0f, height), 0);
		velocity = main_direction * 0.0003f;


		model_matrix = mat4::translate(pos) * mat4::scale(scale);
	}

	void pos_update(vec3 new_pos)
	{
		position = new_pos;
	}

	void update()
	{
		previous_time = current_time;
		current_time = (float)glfwGetTime();

		const float dwTime = current_time - previous_time;

		vec3 eye = vec3(0);
		camera_dis = length(pos - eye);

		velocity += 0.0005f * gravity * dwTime;
		pos += velocity;

		scale -= 0.001f;

		model_matrix = mat4::translate(pos) * mat4::scale(scale);

		float life_factor = 1.0f;
		life -= life_factor * dwTime;

		// disappear
		if (life < 0.0f)
		{
			constexpr float alpha_factor = 0.5f;
			color.a -= alpha_factor * dwTime;
			//printf("color: %f\n", color.a);
		}
		// dead
		if (color.a <= 0.0f || scale <= 0.0f) reset();
	}

	bool particle_t::operator<(const particle_t that) const
	{
		return this->camera_dis > that.camera_dis;
	}
};


struct particles_t
{
	const int max_particle = 500;

	vec3 position;
	vec2 size;

	std::vector<particle_t> particles;
	Texture tex;
	quad_t quad;

	void sort_particle()
	{
		std::sort(&particles[0], &particles[max_particle]);
	}

	void init()
	{
		float aspect = 1024 / (float)512;
		mat4 aspect_matrix =
		{
			min(1 / aspect,1.0f), 0, 0, 0,
			0, min(aspect,1.0f), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		position = vec3(random_range(-1.0f, 1.0f), random_range(-1.0f, 1.0f), random_range(-1.0f, 1.0f));
		size = vec2(1);

		if (!(program_particle = cg_create_program(particle_v_shader_path, particle_f_shader_path))) { printf("Loading map is failed\n"); glfwTerminate(); return; }
		
		glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
		glEnable(GL_CULL_FACE);								// turn on backface culling

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);

		quad.init();
		tex.load("../bin/images/particle/Snowflake.png");

		glUniformMatrix4fv(glGetUniformLocation(program_particle, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);

		// initialize particles
		particles.resize(max_particle);

	}

	void update() 
	{
		for (auto& p : particles) p.position = position;

		glActiveTexture(GL_TEXTURE0);								// select the texture slot to bind
		glBindTexture(GL_TEXTURE_2D, tex.texture[0]);
		glUniform1i(glGetUniformLocation(program_particle, "TEX"), 0);	 // GL_TEXTURE0

		for (auto& p : particles) p.update();
		sort_particle();
	}

	void render()
	{
		glDisable(GL_DEPTH_TEST);
		glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
		glUseProgram(program_particle);
		cg_bind_vertex_attributes(program_particle);

		GLint uloc;
		for (auto& p : particles)
		{
			uloc = glGetUniformLocation(program_particle, "color"); if (uloc > -1) glUniform4fv(uloc, 1, p.color);
			uloc = glGetUniformLocation(program_particle, "model_matrix"); if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, p.model_matrix);

			// render quad vertices
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		glEnable(GL_DEPTH_TEST);
	}
};

#endif


