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

	//control function
	void update(vec3 move);
};

void creature::update()
{

};

void player::update(vec3 move)
{
	model_matrix = mat4::translate(move) * model_matrix;
}



#endif