#pragma once
#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include "cgmath.h"			// slee's simple math library
#include "cgut.h"			// slee's OpenGL utility

#include "texture.h"

#include "sphere.h"			// sphere class definition

//*************************************
// global constants

//************************************
//stb_image load
#include "stb_image.h"

struct background
{
	const char* vert_shader_path = "../bin/shaders/circ.vert";
	const char* frag_shader_path = "../bin/shaders/circ.frag";

	struct camera
	{
		vec3	eye = vec3(-10.0f, 2.0f, 5.0f);
		vec3	at = vec3(0);
		vec3	up = vec3(0, 1.0f, 0);
		mat4	view_matrix = mat4::rotate(vec3(0, 0, 1), PI / 2) * mat4::look_at(eye, at, up);

		float	fovy = PI / 4.0f; // must be in radian
		float	aspect_ratio;
		float	dnear = 1.0f;
		float	dfar = 1000.0f;
		mat4	projection_matrix;
	};

	struct light_t
	{
		vec4	position = vec4(0.0f, 0.0f, 0.0f, 1.0f);   // directional light
		vec4	ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
		vec4	diffuse = vec4(0.9f, 0.9f, 0.9f, 1.0f);
		vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	};

	struct material_t
	{
		vec4	ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
		vec4	diffuse = vec4(0.8f, 0.8f, 0.8f, 1.0f);
		vec4	specular = vec4(0.3f, 0.3f, 0.3f, 1.0f);
		float	shininess = 1000.0f;
	};

	//*************************************
	// window objects
	GLFWwindow* window = nullptr;
	ivec2		window_size = ivec2(1024, 576);	// initial window size
	Texture		tex;
	//*************************************
	// OpenGL objects
	GLuint	program = 0;	// ID holder for GPU program
	GLuint	vertex_buffer = 0;	// ID holder for vertex buffer
	GLuint	quad_vert_buffer = 0;
	GLuint	index_buffer = 0;	// ID holder for index buffer

	//*************************************
	// global variables
	int		frame = 0;						// index of rendering frames
	std::vector<sphere_t>	spheres = std::move(create_spheres());

	//*************************************
	// holder of vertices and indices of a unit sphere
	std::vector<vertex>	unit_sphere_vertices;	// host-side vertices
	camera cam;
	light_t light;
	material_t material;

	GLuint	fbo = 0;					// framebuffer objects
	GLuint	SRC = 0;

	//*************************************
	//texture set
	static std::vector<const char*> texture_path;
	std::vector<GLuint> texture;
	GLuint texture1;

	//*************************************
	void update()
	{
		
		// update projection matrix
		cam.aspect_ratio = window_size.x / float(window_size.y);
		cam.projection_matrix = mat4::perspective(cam.fovy, cam.aspect_ratio, cam.dnear, cam.dfar);

		// update uniform variables in vertex/fragment shaders
		GLint uloc;
		uloc = glGetUniformLocation(program, "view_matrix");			if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.view_matrix);
		uloc = glGetUniformLocation(program, "projection_matrix");	if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, cam.projection_matrix);


		glUniform4fv(glGetUniformLocation(program, "light_position"), 1, light.position);
		glUniform4fv(glGetUniformLocation(program, "Ia"), 1, light.ambient);
		glUniform4fv(glGetUniformLocation(program, "Id"), 1, light.diffuse);
		glUniform4fv(glGetUniformLocation(program, "Is"), 1, light.specular);

		// setup material properties
		glUniform4fv(glGetUniformLocation(program, "Ka"), 1, material.ambient);
		glUniform4fv(glGetUniformLocation(program, "Kd"), 1, material.diffuse);
		glUniform4fv(glGetUniformLocation(program, "Ks"), 1, material.specular);
		glUniform1f(glGetUniformLocation(program, "shininess"), material.shininess);
	}

	void draw()
	{
		float t = float(glfwGetTime()) * 0.4f;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (index_buffer)	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

		// bind vertex attributes to your shader program
		cg_bind_vertex_attributes(program);

		int i = 0;
		// render two spheres: trigger shader program to process vertex data
		for (auto& s : spheres)
		{
			// per-sphere update
			s.update(t);

			// update per-sphere uniforms
			GLint uloc;
			uloc = glGetUniformLocation(program, "model_matrix");		if (uloc > -1) glUniformMatrix4fv(uloc, 1, GL_TRUE, s.model_matrix);

			glBindTexture(GL_TEXTURE_2D, tex.texture[i]);
			glUniform1i(glGetUniformLocation(program, "TEX"), 0);


			glUniform1i(glGetUniformLocation(program, "is_not_star"), s.type);
			// per-sphere draw calls
			glDrawElements(GL_TRIANGLES, (72 + 1) * (72 + 1) * 3, GL_UNSIGNED_INT, nullptr);
			i++;
		}

	}

	void draw_quad()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// bind vertex attributes to your shader program

		vertex corners[4];
		corners[0].pos = vec3(-1.0f, -1.0f, 0.0f);	corners[0].tex = vec2(0.0f, 0.0f);
		corners[1].pos = vec3(+1.0f, -1.0f, 0.0f);	corners[1].tex = vec2(1.0f, 0.0f);
		corners[2].pos = vec3(+1.0f, +1.0f, 0.0f);	corners[2].tex = vec2(1.0f, 1.0f);
		corners[3].pos = vec3(-1.0f, +1.0f, 0.0f);	corners[3].tex = vec2(0.0f, 1.0f);
		vertex vertices[6] = { corners[0], corners[1], corners[2], corners[0], corners[2], corners[3] };

		// generation of vertex buffer is the same, but use vertices instead of corners
		glGenBuffers(1, &quad_vert_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vert_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		cg_bind_vertex_attributes(program);

		// render quad vertices
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}


	void render()
	{
		update();
		glUseProgram(program);

		uint PASS_NUMBER = 1;
		glUniform1i(glGetUniformLocation(program, "pass_n"), PASS_NUMBER);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);												// bind frame buffer object
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SRC, 0);	// attach texture to frame buffer object
		glBindTexture(GL_TEXTURE_2D, SRC);
		draw();

		PASS_NUMBER = 2;
		glUniform1i(glGetUniformLocation(program, "pass_n"), PASS_NUMBER);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);													// unbind frame buffer object: render to the default frame buffer
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);						// detach texture from frame buffer object
		glUniform1i(glGetUniformLocation(program, "TEX"), 0);
		glBindTexture(GL_TEXTURE_2D, SRC);
		draw_quad();

		// swap front and back buffers, and display to screen
		glfwSwapBuffers(window);
	}

	void reshape(GLFWwindow* window, int width, int height)
	{
		// set current viewport in pixels (win_x, win_y, win_width, win_height)
		// viewport: the window area that are affected by rendering 
		window_size = ivec2(width, height);
		glViewport(0, 0, width, height);
	}
	std::vector<vertex> create_sphere_vertices(uint N)
	{
		std::vector<vertex> v;
		for (uint k = 0; k <= N; k++)
		{
			float lngi = PI * 2.0f * k / float(N), lns = sin(lngi), lnc = cos(lngi);
			for (uint i = 0; i <= N / 2; i++)
			{
				float lati = PI * 1.0f * i / float(N / 2), lac = cos(lati), las = sin(lati);
				v.push_back({ vec3(las * lnc, las * lns ,lac), vec3(las * lnc, las * lns ,lac), vec2(lngi / (2 * PI), 1.0f - lati / PI) });
			}
		}
		return v;
	}

	void update_vertex_buffer(const std::vector<vertex>& vertices, uint N)
	{
		// clear and create new buffers
		if (vertex_buffer)	glDeleteBuffers(1, &vertex_buffer);	vertex_buffer = 0;
		if (index_buffer)	glDeleteBuffers(1, &index_buffer);	index_buffer = 0;

		// check exceptions
		if (vertices.empty()) { printf("[error] vertices is empty.\n"); return; }

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

	bool user_init()
	{
		// init GL states
		glLineWidth(1.0f);
		glClearColor(39 / 255.0f, 40 / 255.0f, 34 / 255.0f, 1.0f);	// set clear color
		glEnable(GL_CULL_FACE);								// turn on backface culling
		glEnable(GL_DEPTH_TEST);								// turn on depth tests
		glEnable(GL_TEXTURE_2D);

		unit_sphere_vertices = std::move(create_sphere_vertices(72));
		update_vertex_buffer(unit_sphere_vertices, 72);

		glActiveTexture(GL_TEXTURE0);

		tex.window_size = window_size;

		tex.load("../bin/images/2k_sun.jpg");
		tex.load("../bin/images/2k_mercury.jpg");
		tex.load("../bin/images/2k_venus.jpg");
		tex.load("../bin/images/2k_earth.jpg");
		tex.load("../bin/images/2k_mars.jpg");
		tex.load("../bin/images/2k_jupiter.jpg");
		tex.load("../bin/images/2k_saturn.jpg");
		tex.load("../bin/images/2k_uranus.jpg");
		tex.load("../bin/images/2k_neptune.jpg");

		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glGenTextures(1, &SRC);
		glBindTexture(GL_TEXTURE_2D, SRC);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8 /* GL_RGB for legacy GL */, window_size.x, window_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		return true;
	}


};
#endif