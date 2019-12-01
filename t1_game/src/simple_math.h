#pragma once
#ifndef __SIMPLE_MATH_H__
#define __SIMPLE_MATH_H__

#include "game_object.h"

inline float randf() { return (float)std::rand() / RAND_MAX; }
inline float randf(float m, float p) { return m * randf() + p * randf(); }

inline float dis(creature_t x, creature_t y) { return (x.position - y.position).length(); }
inline float dis(circle_t x, circle_t y) { return (x.position - y.position).length(); }

inline void delay(float secs)
{
	float end = (float)glfwGetTime() + secs;
	while ((float)glfwGetTime() < end);
}

inline void calculate(circle_t& v1, circle_t& v2)
{
	vec2 temp = v1.velocity;
	v1.velocity = v1.velocity - 2 * (v2.mass / (v1.mass + v2.mass)) * (dot((v1.velocity - v2.velocity), (v1.position - v2.position)) / length2(v1.position - v2.position)) * (v1.position - v2.position);
	v2.velocity = v2.velocity - 2 * (v1.mass / (v1.mass + v2.mass)) * (dot((v2.velocity - temp), (v2.position - v1.position)) / length2(v2.position - v1.position)) * (v2.position - v1.position);
}

inline void calculate(creature_t& v1, creature_t& v2)
{
	vec3 temp = v1.velocity;

	//vec2 pos_temp = vec2(v1.position.x - v2.position.x, v1.position.y - v2.position.y);

	v1.velocity = v1.velocity - 2 * (v2.mass / (v1.mass + v2.mass)) * (dot((v1.velocity - v2.velocity), (v1.position - v2.position)) / length2(v1.position - v2.position)) * (v1.position - v2.position);
	v2.velocity = v2.velocity - 2 * (v1.mass / (v1.mass + v2.mass)) * (dot((v2.velocity - temp), (v2.position - v1.position)) / length2(v2.position - v1.position)) * (v2.position - v1.position);
}

inline bool check_collision(player_t& p, creature_t& c) // AABB - AABB collision
{
	bool x_axis_collision = p.position.x + p.size.x >= c.position.x && c.position.x + c.size.x >= p.position.x;
	bool y_axis_collision = p.position.y + p.size.y >= c.position.y && c.position.y + c.size.y >= p.position.y;

	return x_axis_collision && y_axis_collision;
}



#endif

