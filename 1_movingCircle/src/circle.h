#pragma once
#ifndef __CIRCLE_H__
#define __CIRCLE_H__

struct circle_t
{
	uint	index;
	vec2	center=vec2(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	vec4	color;				// RGBA color in [0,1]

	vec2	velocity = vec2(0.1f);
	float	mass = (float)pow(radius, 2.0) * PI;

	uint	collision[2] = {0, FALSE}; // c1, c2, check collsion

	mat4	model_matrix;		// modeling transformation
	void	update( float t );
};

float randnum(void) { return (float)(rand() - RAND_MAX / 2) / (RAND_MAX/2); }
float dis(vec2 a, vec2 b) { return (float)sqrt((pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0))); }


inline void caculate() {};
bool checkCollision(circle_t a, circle_t b) { if (dis(a.center, b.center) <= a.radius + b.radius) return TRUE; else return FALSE; }
void checkCollision(std::vector<circle_t>& circles)
{
	printf("size:%u\n", circles.size());
	int count = 1;
	uint i, j;
	for (j = 0; j < circles.size(); j++)
	{
		for (i = j + 1; i < circles.size(); i++)
		{
			if (dis(circles[i].center, circles[j].center) <= circles[i].radius + circles[j].radius)
			{
				circles[i].collision[0] = j;
				circles[i].collision[1] = TRUE;
				printf("between%i, %i, dis:%f, radiss:%f, count:%i\n", i, j, dis(circles[i].center, circles[j].center), circles[i].radius + circles[j].radius, count);
				printf("circle %u with %u, collsion%u\n", circles[i].index, circles[i].collision[0], circles[i].collision[1]);
				count += 1;
			}
		}
	}
}


inline std::vector<circle_t> create_circles()
{
	std::vector<circle_t> circles;
	circle_t c;
	
	for (uint i = 0; i < 5; i++)
	{
		c = { i, vec2(randnum(),randnum()),abs(randnum() / 2.0f),vec4(abs(randnum()),abs(randnum()),abs(randnum()),1.0f), vec2(randnum(),randnum()) };
		//c.parent = circles;
		//printf("test.index: %u", c.parent.at(1).index);
		circles.emplace_back(c);
	}
	checkCollision(circles);
	return circles;
}

inline void circle_t::update(float t)
{
	vec2 center_u = velocity * t + center;

	if (collision[1]) { caculate();  collision[1] = TRUE; }
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
