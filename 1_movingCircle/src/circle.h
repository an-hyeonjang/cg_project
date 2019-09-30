#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	uint	index;				// index
	bool	crash;				// if it crashed
	uint	c_circle_index = 21;		// crashed circle index
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	vec2	velocity = vec2(0.1f);
	float	mass = radius;

	mat4	model_matrix;		// modeling transformation
	void	update( float t ); 
};

struct wall_t
{
	uint	position;
	vec2	velocity = vec2(0.0f);
	float	mass;
};


inline float randf() 
{ 
	return (float)std::rand()/RAND_MAX;	//return positive value
}

inline float randf(float m, float p)
{
	return m*randf() + p*randf();
}

inline float dis(circle_t x, circle_t y)
{
	return (x.center - y.center).length();
}

inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;

	for (uint k = 0; k < 1; k++)
	{
		circle_t c, b;
		c.index = k;
		c.center = vec2(randf(-1.0f,1.0f), randf(-1.0f, 1.0f));
		c.radius = randf()/5.0f; 
		c.color = vec4(randf(), randf(), randf(), 1.0f);
		c.velocity = vec2(randf(-1.0f,1.0f)/255.0f, randf(-1.0f,1.0f)/255.0f);

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
		wall = { i, vec2(0.0f), 10.0f};
		window.emplace_back(wall);
	}
	return  window;
}

inline void circle_t::update(float t)
{
	center += velocity;

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
