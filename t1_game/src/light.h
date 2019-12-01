#pragma once
#ifndef __light_h__
#define __light_h__

struct light_t
{
	vec4	position = vec4(1.0f, 1.0f, 1.0f, 1.0f);   // directional light
	vec4	ambient = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4	diffuse = vec4(0.9f, 0.9f, 0.9f, 1.0f);
	vec4	specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
};

struct material_t
{
	vec4	ambient = vec4(0.1f, 0.1f, 0.5f, 0.3f);
	vec4	diffuse = vec4(0.2f, 0.2f, 0.75f, 0.9f);
	vec4	specular = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	float	shininess = 1000.0f;
};

#endif // !__light_h__
