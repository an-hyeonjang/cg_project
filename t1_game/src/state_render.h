#pragma once
#ifndef __STAGE_RENDER_H__
#define __STAGE_RENDER_H__

#include "game_level.h"

background_t menu_background;

void text_init();
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color);
void render_text(std::string text, GLint x, GLint y, GLfloat scale, vec4 color, float t, GLFWwindow* window);

void text_box_render(const char* texture_path)
{
	Texture text_box;

	text_box.load(texture_path);

	glUseProgram(program_object);

	glBindBuffer(GL_ARRAY_BUFFER, quad.vertex_buffer);
	cg_bind_vertex_attributes(program_object);

	mat4 model_matrix = mat4::translate(vec3(0, 0.5f, z_depth + 0.1f)) * mat4::scale(1.5f, 0.5f, 0);

	glUniformMatrix4fv(glGetUniformLocation(program_object, "aspect_matrix"), 1, GL_TRUE, aspect_matrix);
	glUniformMatrix4fv(glGetUniformLocation(program_object, "model_matrix"), 1, GL_TRUE, model_matrix);
	
	glBindTexture(GL_TEXTURE_2D, text_box.texture[0]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);

}


#endif // __STAGE_RENDER_H__