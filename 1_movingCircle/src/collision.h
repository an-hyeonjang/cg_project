#pragma once

#include <iostream>
#include "circle.h"

void calculate(circle_t& v1, wall_t& v2)
{
	vec2 left, down, right, up, t;
	left = vec2(-1.5f, v1.center.y) - v1.center;  //v2-v1
	down = vec2(v1.center.x, -1.0f) - v1.center;
	right = vec2(1.5f, v1.center.y) - v1.center;
	up = vec2(v1.center.x, 1.0f) - v1.center;

	t = v1.velocity;
	if (v2.position == 0)
	{
		v1.velocity = v1.velocity - (2 * v2.mass / (v1.mass + v2.mass)) * ((v1.velocity - v2.velocity) * (-left) / length2(-left)) * (-left);
		v2.velocity = v2.velocity - (2 * v1.mass / (v2.mass + v1.mass)) * ((v2.velocity - t) * left / length2(left)) * (left);
	}
	if (v2.position == 1)
	{
		v1.velocity = v1.velocity - (2 * v2.mass / (v1.mass + v2.mass)) * ((v1.velocity - v2.velocity) * (-down) / length2(-down)) * (-down);
		v2.velocity = v2.velocity - (2 * v1.mass / (v2.mass + v1.mass)) * ((v2.velocity - t) * (down) / length2(down)) * (down);
	}
	if (v2.position == 2)
	{
		v1.velocity = v1.velocity - (2 * v2.mass / (v1.mass + v2.mass)) * ((v1.velocity - v2.velocity) * (-right) / length2(-right)) * (-right);
		v2.velocity = v2.velocity - (2 * v1.mass / (v2.mass + v1.mass)) * ((v2.velocity - t) * (right) / length2(right)) * (right);
	}
	if (v2.position == 3)
	{
		v1.velocity = v1.velocity - (2 * v2.mass / (v1.mass + v2.mass)) * ((v1.velocity - v2.velocity) * (-up) / length2(-up)) * (-up);
		v2.velocity = v2.velocity - (2 * v1.mass / (v2.mass + v1.mass)) * ((v2.velocity - t) * (up) /length2(up)) * (up);
	}
};

inline void calculate(circle_t& v1, circle_t& v2)
{
	vec2 temp = v1.velocity;
	v1.velocity = v1.velocity - 2 * v2.mass / (v1.mass + v2.mass) * ((v1.velocity - v2.velocity) * (v1.center - v2.center)) / length2(v1.center - v2.center) * (v1.center - v2.center);
	v2.velocity = v2.velocity - 2 * v1.mass / (v1.mass + v2.mass) * ((v2.velocity - temp) * (v2.center - v1.center)) / length2(v2.center - v1.center) * (v2.center - v1.center);
};
inline void wall_Collision(circle_t& c, std::vector<wall_t>& window)
{
		if (abs(-1.5f - c.center.x) <= c.radius){ calculate(c, window[0]); }
		if (abs(-1.0f - c.center.y) <= c.radius){ calculate(c, window[1]); }
		if (abs(1.5f - c.center.x) <= c.radius) { calculate(c, window[2]); }
		if (abs(1.0f - c.center.y) <= c.radius) { calculate(c, window[3]); }
}

inline void obj_Collision(circle_t& c, std::vector<circle_t>& circles)
{
	for (uint i = c.index + 1; i < circles.size(); i++)
	{
		if (c.c_circle_index == 21 || c.c_circle_index != i)
		{
			if (length(c.center - circles[i].center) <= c.radius + circles[i].radius)
			{
				c.crash = TRUE;
				c.c_circle_index = i;
			}
		}
	}
}

