#pragma once

#include "circle.h"

inline void calculate(circle_t& c, wall_t& w)
{
	vec2 left, down, right, up;
	left = vec2(-1.5f - c.center.x, 0.0f);
	down = vec2(0.0f , -1.0f - c.center.y);
	right = vec2(1.5f - c.center.x, 0.0f);
	up = vec2(0.0f, 1.0f - c.center.y);
	if(w.position == 0)
		c.velocity = c.velocity - 2 * w.mass / (c.mass + w.mass) * (c.velocity * left) / length2(left) * (left);
	if (w.position == 1)
		c.velocity = c.velocity - 2 * w.mass / (c.mass + w.mass) * (c.velocity * down) / length2(down) * (down); 
	if (w.position == 2)
		c.velocity = c.velocity - 2 * w.mass / (c.mass + w.mass) * (c.velocity * right) / length2(right) * (right);
	if (w.position == 3)
		c.velocity = c.velocity - 2 * w.mass / (c.mass + w.mass) * (c.velocity * up) / length2(up) * (up);
};

inline void calculate(circle_t& v1, circle_t& v2)
{
	v1.velocity = v1.velocity - 2 * v2.mass / (v1.mass + v2.mass) * ((v1.velocity - v2.velocity) * (v1.center - v2.center)) / length2(v1.center - v2.center) * (v1.center - v2.center);
	v2.velocity = v2.velocity - 2 * v2.mass / (v1.mass + v2.mass) * ((v2.velocity - v1.velocity) * (v2.center - v1.center)) / length2(v2.center - v1.center) * (v2.center - v1.center);
};

inline void wall_Collision(circle_t& c, std::vector<circle_t>& circles, std::vector<wall_t>& window)
{
	for (uint i = c.index; i < circles.size(); i++)
	{
		if (abs(-1.5f - c.center.x) <= c.radius) { calculate(c, window.at(0)); }
		if (abs(-1.0f - c.center.y) <= c.radius) { calculate(c, window.at(1)); }
		if (abs(1.5f - c.center.x) <= c.radius) { calculate(c, window.at(2)); }
		if (abs(1.0f - c.center.y) <= c.radius) { calculate(c, window.at(3)); }
	}
}

inline void checkCollision(circle_t& c, std::vector<circle_t>& circles)
{
	for (uint i = c.index; i < circles.size(); i++)
	{
		if (length(c.center - circles[i].center) <= c.radius + circles[i].radius) { c.crash = TRUE; c.c_circle_index = i; }
	}
}