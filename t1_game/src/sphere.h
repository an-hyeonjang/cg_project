#pragma once
#ifndef __SPHERE_H__
#define __SPHERE_H__

struct sphere_t
{
	vec3	center=vec3(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	float	speed=1.0f;
	float	theta=0.0f;			// rotation angle
	GLuint	type = 1;

	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update( float t );	
};

inline std::vector<sphere_t> create_spheres()
{
	std::vector<sphere_t> spheres;
	sphere_t sun;
	sphere_t mecury, venus, earth, mars, jupiter, saturn, uranus, neptune;
	
	sun = { vec3(0.0f, 0.0f, 0.0f),1.0f,0.8f };
	sun.type = 0;
	spheres.emplace_back(sun);
	
	mecury = { vec3(1.2f,-1.0f,0.0f),0.05f, 4.7f};
	spheres.emplace_back(mecury);

	venus = { vec3(-1.5f,1.2f,0.0f),0.2f, 3.5f };
	spheres.emplace_back(venus);

	earth = { vec3(2.3f,-1.0f,0.0f),0.15f, 2.9f };
	spheres.emplace_back(earth);

	mars = { vec3(-3.04f,1.02f,0.0f),0.1f, 2.4f };
	spheres.emplace_back(mars);

	jupiter = { vec3(5.4f,-1.4f,0.0f),0.7f, 1.3f };
	spheres.emplace_back(jupiter);

	saturn = { vec3(-8.0f,1.0f,0.0f),0.5f, 0.9f };
	spheres.emplace_back(saturn);

	uranus = { vec3(9.0f,1.0f,0.0f),0.16f, 0.5f };
	spheres.emplace_back(uranus);

	neptune = { vec3(9.9f,-1.9f,0.0f),0.18f, 0.4f };
	spheres.emplace_back(neptune);

	return spheres;
}

inline void sphere_t::update( float t )
{
	float sp = speed;
	theta	= t;
	float c	= cos(theta), s= sin(theta);
	

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix = mat4::scale(radius);
	mat4 rotation_matrix = mat4::rotate(vec3(0,0,1),theta);
	mat4 translate_matrix = mat4::translate(center);
	
	mat4 revolution = mat4::rotate(vec3(0, 0, 1), sp * theta);

	model_matrix = translate_matrix * rotation_matrix * scale_matrix;
	model_matrix = type?  revolution * model_matrix : model_matrix;
}

#endif
