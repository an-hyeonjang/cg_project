#pragma once
#ifndef __GAME_LEVEL_H_
#define	__GAME_LEVEL_H_

#include "game_object.h"
#include "simple_math.h"

bool change_state(player_t& p, std::vector<creature_t>& creatures)
{
	for (uint i = 0; i < creatures.size(); i++)
	{
		if (check_collision(p, creatures[i]))
		{
			if (p.hit_on)
			{
				creatures.erase(creatures.begin() + i);
				engine->play2D(bubble_effect_src, false);
			}
		}
	}
	if (creatures.empty()) return true;
	else return false;
}

inline void wall_collision(creature_t& c, wall_t& wall)
{
	//counter-clockwise
	if (abs(wall.left - c.position.x) <= c.size.x || c.position.x < wall.left)
	{
		c.position.x += c.size.x - c.position.x + wall.left;
		c.velocity = vec3(-c.velocity.x, c.velocity.y, c.velocity.z);
	}
	if (abs(wall.bottom - c.position.y) <= c.size.x || c.position.y < wall.left)
	{
		c.position.y += c.size.x - c.position.y + wall.bottom;
		c.velocity = vec3(c.velocity.x, -c.velocity.y, -c.velocity.z);
	}
	if (abs(wall.right - c.position.x) <= c.size.x || c.position.x > wall.right)
	{
		c.position.x -= c.size.x + c.position.x - wall.right;
		c.velocity = vec3(-c.velocity.x, c.velocity.y, c.velocity.z);
	}
	if (abs(wall.top - c.position.y) <= c.size.x || c.position.y > wall.top)
	{
		c.position.y -= c.size.x + c.position.y - wall.top;
		c.velocity = vec3(c.velocity.x, -c.velocity.y, -c.velocity.z);
	}
}

void object_collision(creature_t& c, std::vector<creature_t>& cs)
{
	for (uint i = c.index + 1; i < cs.size(); i++)
	{
		float distance = dis(c, cs[i]);
		float t = c.size.x + cs[i].size.x - distance;
		vec3 error = { t / distance * (c.position.x - cs[i].position.x) , t / distance * (c.position.y - cs[i].position.y), 0 };

		if (t >= 0)
		{
			c.position += error / 2;
			cs[i].position -= error / 2;
			calculate(c, cs[i]);
		}
	}
}

inline std::vector<creature_t> create_creatures(uint size, float max_scale)
{
	std::vector<creature_t> creatures;
	creature_t c;

	for (uint k = 0; k < size; k++)
	{
		creature_t c;
		c.init(randf(0.005f, max_scale));
		c.index = k;
		c.position = vec3(randf(-1.0f, 1.0f), randf(-0.65f, 0.65f), c.position.y);
		c.velocity = vec3(randf(-1.5f, 1.5f) * c.mass, randf(-1.5f, 1.5f) * c.mass, randf(-1.5f, 1.5f) * c.mass);

		bool b_collision = FALSE;
		for (uint i = 0; i < creatures.size(); i++)
		{
			if (dis(c, creatures[i]) <= c.size.x + creatures[i].size.x)
				b_collision = TRUE;
			while (b_collision)
			{
				c.position = vec3(randf(-1.0f, 1.0f), randf(-1.0f, 1.0f), c.position.y);
				for (uint j = 0; j < creatures.size(); j++)
				{
					if (dis(c, creatures[j]) <= c.size.x + creatures[j].size.x)
					{
						b_collision = TRUE;
						break;
					}
					else b_collision = FALSE;
				}
			}
		}
		creatures.emplace_back(c);
	}
	return creatures;
}

void tracking_player(creature_t& c, player_t& p, float t)
{
	float timer = 0.0f;
	vec3 temp_pos = p.position;
	if (t > 20.0f)
	{
		c.position += temp_pos / 10.0f;
		if (c.position == temp_pos) return;
	}
}

void tracking_player(particles_t& ptc, player_t& p, float t)
{
	float timer = 0.0f;
	vec3 temp_pos;

	float s = sin(t) * 10;

	temp_pos = p.position;
	if (s > 6.5f && s < 8.0f)
	{
		if (ptc.position.x == temp_pos.x) return;
		ptc.position.x += temp_pos.x/20.0f;//temp_pos / 10.0f;
		ptc.position.y += temp_pos.y/20.0f;
		
	}
	return;
}

#endif