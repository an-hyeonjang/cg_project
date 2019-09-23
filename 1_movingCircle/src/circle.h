#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__


float randnum() { return (float)(rand() - RAND_MAX / 2) / (RAND_MAX / 2); }

struct circle_t
{
	uint	index;				// index
	bool	crash;				// if it crashed
	uint	c_circle_index;		// crashed circle index
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	vec2	velocity = vec2(0.1f);
	float	mass = (float)pow(radius, 2.0) * PI;

	mat4	model_matrix;		// modeling transformation
	void	update( float t ); 
};

struct wall_t
{
	uint	position;		// counter clock-wise, 0_left 1_down 2_right 3_up
	vec2	velocity = vec2(0.0f);
	float	mass;
};

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	
	for (uint i = 0; i < 10; i++)
	{
		c = { i , FALSE, 0 , vec2(randnum(), randnum()), abs(randnum() / 4.0f), vec4(abs(randnum()),abs(randnum()),abs(randnum()),1.0f), vec2(randnum(),randnum()) };
		circles.emplace_back(c);
	}
	return circles;
}

inline std::vector<wall_t> create_walls()
{
	std::vector<wall_t> window;
	wall_t wall;
	for (uint i = 0; i < 4; i++)
	{
		wall = { i, vec2(0.0f), 10.0f };
		window.emplace_back(wall);
	}
	return  window;
}

inline void circle_t::update(float t)
{
	center += velocity/255 ;

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix =
	{
		radius, 0, 0, 0,
		0, radius, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 rotation_matrix =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	mat4 translate_matrix =
	{
		1, 0, 0, center.x,
		0, 1, 0, center.y,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}

#endif
