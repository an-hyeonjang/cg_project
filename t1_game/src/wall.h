#pragma once
#ifndef __WALL_H_
#define __WALL_H_

struct wall_t
{
	float	left;
	float	right;
	float	top;
	float	bottom;

	void	init(ivec2 size);
};

inline void wall_t::init(ivec2 size)
{
	if (size.x >= size.y)
	{
		float aspect = size.x / (float)size.y;
		left = -1.0f * aspect;
		right = 1.0f * aspect;
		top = 1.0f;
		bottom = -1.0f;
	}
	else if (size.y > size.x)
	{
		float aspect = size.y / (float)size.x;
		left = -1.0f;
		right = 1.0f;
		top = 1.0f * aspect;
		bottom = -1.0f * aspect;
	}
}

#endif