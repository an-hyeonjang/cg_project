#version 330

// input from vertex shader
in vec4 epos;
in vec3 norm;
in vec2 tc;

// the only output variable
out vec4 fragColor;

// uniform variables
uniform mat4	view_matrix;
uniform vec4	light_position, Ia, Id, Is;	// light
uniform vec4	Ka, Kd, Ks;					// material properties
uniform float	shininess;

uniform uint qti;

float stepdiv(float val, float div)
{
	for (float i = 0; i < div; i++)
	{
		if (val >= i/div && val < (i + 1)/div ) 
			return val = (i/div);
	}
}

vec4 cel( vec3 l, vec3 n, vec3 h, vec4 Kd, uint dk )
{
	float d = stepdiv(dot(l,n), float(dk)) + 0.75f;
	vec4 Ird = max(d*Kd*Id,0.0);					// diffuse reflection
	return Ird;
}

void main()
{
	// light position in the eye space
	vec4 lpos = view_matrix*light_position;

	vec3 n = normalize(norm);	// norm interpolated via rasterizer should be normalized again here
	vec3 p = epos.xyz;			// 3D position of this fragment
	vec3 l = normalize(lpos.xyz-(lpos.a==0?vec3(0):p));	// lpos.a==0 means directional light
	vec3 v = normalize(-p);		// eye-epos = vec3(0)-epos
	vec3 h = normalize(l+v);	// the halfway vector
	
	fragColor = cel( l, n, h, Kd, qti);
}