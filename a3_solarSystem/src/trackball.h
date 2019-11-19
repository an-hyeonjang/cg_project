#ifndef __TRACKBALL_H__
#define __TRACKBALL_H__
#include "cgmath.h"

struct trackball
{
	int		button;
	int		mods;
	bool	b_tracking = false;
	float	scale;			// controls how much rotation is applied
	mat4	view_matrix0;	// initial view matrix
	vec2	m0;				// the last mouse position

	trackball( float rot_scale=1.0f ):scale(rot_scale){}
	bool is_tracking() const { return b_tracking; }
	void begin( const mat4& view_matrix, float x, float y )
	{
		b_tracking = true;			// enable trackball tracking
		m0 = vec2(x,y)*2.0f-1.0f;	// convert (x,y) in [0,1] to [-1,1]
		view_matrix0 = view_matrix;	// save current view matrix
	}
	void end() { b_tracking = false; }

	mat4 t_update( float x, float y )
	{
		// retrive the current mouse position
		vec2 m = vec2(x,y)*2.0f - 1.0f; // normalize xy

		// project a 2D mouse position to a unit sphere
		static const vec3 p0 = vec3(0,0,1.0f);	// reference position on sphere
		vec3 p1 = vec3(m.x-m0.x, m0.y-m.y,0);	// displacement with vertical swap
		if (!b_tracking || length(p1) < 0.0001f) return view_matrix0;			// ignore subtle movement
		p1 *= scale;												// apply rotation scale
		p1 = vec3(p1.x,p1.y,sqrtf(max(0,1.0f-length2(p1)))).normalize();	// back-project z=0 onto the unit sphere

		// find rotation axis and angle
		// - right-mult of mat3 = inverse view rotation to world
		// - i.e., rotation is done in the world coordinates
		vec3 n = p0.cross(p1)*mat3(view_matrix0);
		float angle = asin( min(n.length(),1.0f) );

		return view_matrix0 * mat4::rotate(n.normalize(),angle);
	}

	mat4 p_update(vec3 eye, vec3 at, vec3 up, float x, float y)
	{
		vec2 m = vec2(x, y) * 2.0f - 1.0f;
		vec3 p1 = vec3(0, m0.x-m.y, m0.y-m.y);

		if( p1.y * p1.y > p1.z * p1.z ) p1 = vec3(0, m0.x - m.x, 0);
		else if (p1.y * p1.y < p1.z* p1.z) p1 = vec3(0, 0, m0.y - m.y);

		p1 *= scale;

		return mat4::rotate(vec3(0, 0, 1), PI / 2) * mat4::look_at(eye - p1, at - p1, up) * mat4::look_at(eye, at, up).inverse() * mat4::rotate(vec3(0, 0, 1), -PI / 2) * view_matrix0;
	}

	mat4 z_update(float x, float y)
	{
		float dis = 1.0f;
		vec2 m = vec2(x, y) * 2.0f - 1.0f;

		if (m.y - m0.y < 0) dis = -length(m0 - m);
		else if (m.y - m0.y >= 0) dis = length(m0 - m);

		vec3 p1 = 3.0f * scale * vec3(0, 0, dis);
		if (!b_tracking || length(p1) < 0.0001f || length(p1) > 20.0f) return view_matrix0;

		return mat4::translate(p1) * view_matrix0;
	}
};

#endif // __TRACKBALL_H__
