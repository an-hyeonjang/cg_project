#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	uint	index;				// index
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	vec2	velocity = vec2(0.1f);
	float	mass = radius*radius;

	mat4	model_matrix;		// modeling transformation
	void	update( float t ); 
	float	speed_scale = 150.0f;
};

struct wall_t
{
	float	left;
	float	right; 
	float	top;
	float	bottom;

	void	update(ivec2 size);
};

inline void wall_t::update(ivec2 size)
{
	
	if (size.x >= size.y)
	{
		float aspect = size.x / (float)size.y;
		left = -1.0f * aspect;
		right = 1.0f * aspect;
		top = 1.0f;
		bottom = -1.0f;
	}
	else if(size.y > size.x)
	{
		float aspect = size.y / (float)size.x;
		left = -1.0f;
		right = 1.0f;
		top = 1.0f * aspect;
		bottom = -1.0f * aspect;
	}
}

inline float randf() //return positive random value
{ 
	return (float)std::rand()/RAND_MAX; 
} 

inline float randf(float m, float p) //return ranged random value
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

	for (uint k = 0; k < 20; k++)
	{
		circle_t c;
		c.index = k;
		c.center = vec2(randf(-1.0f,1.0f), randf(-0.65f, 0.65f));
		c.radius = randf(0.05f, 0.2f);
		c.color = vec4(randf(), randf(), randf(), 1.0f);
		c.velocity = vec2(randf(-1.5f,1.5f)*c.mass, randf(-1.5f,1.5f)*c.mass);

		bool b_collision = FALSE;
		for (uint i = 0; i < circles.size(); i++)
		{
			if (dis(c, circles[i]) <= c.radius + circles[i].radius)
				b_collision = TRUE;
			while (b_collision) 
			{
				c.center = vec2(randf(-1.0f, 1.0f), randf(-1.0f, 1.0f));
				for ( uint j = 0; j < circles.size(); j++ )
				{
					if (dis(c, circles[j]) <= c.radius + circles[j].radius)
					{
						b_collision = TRUE; 
						break;
					}
					else b_collision = FALSE;
				}
			}
		}
		circles.emplace_back(c);
	}
	return circles;
}

inline void circle_t::update(float t)
{
	center += velocity/speed_scale;

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

inline void calculate(circle_t& v1, circle_t& v2)
{
	vec2 temp = v1.velocity;
	v1.velocity = v1.velocity - 2 * (v2.mass / (v1.mass + v2.mass)) * (dot((v1.velocity - v2.velocity), (v1.center - v2.center)) / length2(v1.center - v2.center)) * (v1.center - v2.center);
	v2.velocity = v2.velocity - 2 * (v1.mass / (v1.mass + v2.mass)) * (dot((v2.velocity - temp), (v2.center - v1.center)) / length2(v2.center - v1.center)) * (v2.center - v1.center);
}

inline void wall_Collision(circle_t& c, wall_t& wall)
{

	//counter-clockwise
	if (abs( wall.left - c.center.x) <= c.radius || c.center.x < wall.left )
	{
		c.center.x += c.radius - c.center.x + wall.left;
		c.velocity = vec2(-c.velocity.x, c.velocity.y);
	}
	if (abs(wall.bottom - c.center.y) <= c.radius || c.center.y < wall.left )
	{
		c.center.y += c.radius - c.center.y + wall.bottom;
		c.velocity = vec2(c.velocity.x, -c.velocity.y);
	}
	if (abs(wall.right - c.center.x) <= c.radius || c.center.x > wall.right )
	{
		c.center.x -= c.radius + c.center.x - wall.right;
		c.velocity = vec2(-c.velocity.x, c.velocity.y);
	}
	if (abs(wall.top - c.center.y) <= c.radius || c.center.y > wall.top )
	{
		c.center.y -= c.radius + c.center.y - wall.top;
		c.velocity = vec2(c.velocity.x, - c.velocity.y);
	}
}

inline void obj_Collision(circle_t& c, std::vector<circle_t>& circles)
{
	for (uint i = c.index + 1; i < circles.size(); i++)
	{
		float distance = dis(c, circles[i]);
		float t = c.radius + circles[i].radius - distance;
		vec2 error = { t/distance * (c.center.x - circles[i].center.x) , t/distance * (c.center.y - circles[i].center.y) };

		if (t >= 0)
		{
			c.center += error / 2;
			circles[i].center -= error / 2;
			calculate(c, circles[i]);
		}
	}
}
#endif
