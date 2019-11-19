#version 330

in vec3 norm;
in vec2 tc;	
in vec4 epos;

out vec4 fragColor;

uniform int		b_solid_color;
uniform mat4	view_matrix;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties
uniform float	shininess;

uniform sampler2D TEX;
uniform bool	shading;
uniform bool	is_not_star;

vec4 phong( vec3 l, vec3 n, vec3 h, vec4 Kd )
{
	vec4 Ira = Ka*Ia;									// ambient reflection
	vec4 Ird = max(Kd*dot(l,n)*Id,0.0);					// diffuse reflection
	vec4 Irs = max(Ks*pow(dot(h,n),shininess)*Is,0.0);	// specular reflection
	return Ira + Ird + Irs;
}

vec4 tex(int select)
{
	if(select == 0) return vec4(tc.xy,0,1);
	else if(select == 1) return vec4(tc.xxx, 1);
	else if(select == 2) return vec4(tc.yyy, 1);
}

void main()
{
	vec4 lpos = view_matrix*light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector

	vec4 Kd = texture2D( TEX, tc );
	
	if(!shading) fragColor = tex(b_solid_color);
	else if(shading)
	{
		if(!is_not_star) fragColor = Kd;
		else fragColor = phong(l, n, h, Kd);
	}
}

