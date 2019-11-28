#pragma once
#ifndef __GAME_OBJECT_H_
#define	__GAME_OBJECT_H_

#include "texture.h"
#include "quad.h"
#include "circle.h"

struct map
{

};

struct creature
{
	//model and images;
	quad_t	model;
	Texture	tex;

	//attribute for games;
	uint	life;

	//control function
	void update();
};

struct player
{
	//model and images;
	quad_t	model;
	Texture	tex;
	float	scale;

	//attribute for games;
	uint	life;
	
	//
	mat4 model_matrix;

	player(float size = 1.0f):scale(size) { model_matrix = mat4::scale(size); }

	void init();
	//control function
	void update(vec3 move);

	void render();
};

void player::init()
{

}


void player::update(vec3 move)
{
	mat4 scale_matrix = mat4::scale(scale);
	mat4 rotation_matrix = mat4::rotate(vec3(0, 0, 1), 1);
	mat4 translate_matrix = mat4::translate(0);

	model_matrix = translate_matrix * rotation_matrix * scale_matrix;

	model_matrix = mat4::translate(move) * model_matrix;
}


void player::render()
{

}


void creature::update()
{

}








#endif