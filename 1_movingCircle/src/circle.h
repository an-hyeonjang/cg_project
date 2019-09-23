#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

#include <iostream>

struct circle_t
{
	uint	index;
	bool	crash;
	uint	c_circle_index; // index, if collision, with which?
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	vec2	velocity = vec2(0.1f);
	float	mass = (float)pow(radius, 2.0) * PI;

	mat4	model_matrix;		// modeling transformation
	void	update( float t ); 
};

struct wall
{
	vec2	center;
	vec2	velocity = vec2(0.0f);
	float	mass;
};


float randnum() { return (float)(rand() - RAND_MAX/2) / (RAND_MAX/2); }

inline void calculate(circle_t& v1, wall& v2)
{
	v1.velocity = v1.velocity - 2 * v2.mass / (v1.mass + v2.mass) * ((v1.velocity - v2.velocity) * (v1.center - v2.center)) / length2(v1.center - v2.center) * (v1.center - v2.center);
};

inline void calculate(circle_t& v1, circle_t& v2) 
{
	v1.velocity = v1.velocity - 2 * v2.mass / (v1.mass + v2.mass) * ((v1.velocity - v2.velocity) * (v1.center - v2.center)) / length2(v1.center - v2.center) * (v1.center - v2.center);
	v2.velocity = v2.velocity - 2 * v2.mass / (v1.mass + v2.mass) * ((v2.velocity - v1.velocity) * (v2.center - v1.center)) / length2(v2.center - v1.center) * (v2.center - v1.center);
};


inline void checkCollision(circle_t& c, std::vector<circle_t>& circles)
{
	/*for (uint i = c.index; i < circles.size(); i++) 
	{ 
		if (length(c.center - circles[i].center) <= c.radius + circles[i].radius) { c.crash = TRUE; c.c_circle_index = i; } 
	}*/
}
inline void wallCollision(circle_t& c, std::vector<circle_t>& circles)
{
	wall right, left, up, down;
	right = { vec2(1.5f, c.center.y), vec2(0.0f), 10.0f };
	left = { vec2(-1.5f, c.center.y), vec2(0.0f), 10.0f };
	up = { vec2(c.center.x, 1.0f), vec2(0.0f), 10.0f };
	down = { vec2(c.center.x, -1.0f), vec2(0.0f), 10.0f };
	for (uint i = c.index; i < circles.size(); i++)
	{
		if (length(c.center - right.center) <= c.radius) { calculate(c, right); }
		if (length(c.center - left.center) <= c.radius) { calculate(c, left); }
		if (length(c.center - up.center) <= c.radius) { calculate(c, up); }
		if (length(c.center - down.center) <= c.radius) { calculate(c, down); }
	}
}


inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	
	for (uint i = 0; i < 10; i++)
	{
		c = { i , FALSE, 0 , vec2(randnum(), randnum()),abs(randnum() / 4.0f), vec4(abs(randnum()),abs(randnum()),abs(randnum()),1.0f), vec2(randnum(),randnum()) };
		circles.emplace_back(c);
	}
	return circles;
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
