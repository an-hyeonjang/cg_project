#pragma once
#ifndef __SPHERE_H__
#define __SPHERE_H__

struct sphere_t
{
	vec3	center=vec3(0);		// 2D position for translation
	float	radius=1.0f;		// radius
	float	theta=0.0f;			// rotation angle
	vec4	color;				// RGBA color in [0,1]
	mat4	model_matrix;		// modeling transformation

	// public functions
	void	update( float t );	
};

inline std::vector<sphere_t> create_spheres()
{
	std::vector<sphere_t> spheres;
	sphere_t c;
	
	c = {vec3(0),1.0f,0.0f,vec4(1.0f,1.0f,1.0f,1.0f)};
	spheres.emplace_back(c);

	return spheres;
}

inline void sphere_t::update( float t )
{
	theta	= t;
	float c	= cos(theta), s=sin(theta);

	// these transformations will be explained in later transformation lecture
	mat4 scale_matrix = mat4::scale(radius);
	
	mat4 rotation_matrix = mat4::rotate(vec3(0,0,1), theta);

	mat4 translate_matrix = mat4::translate(center);

	model_matrix = translate_matrix*rotation_matrix*scale_matrix;
}

#endif
