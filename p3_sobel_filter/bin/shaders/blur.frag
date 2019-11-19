#version 330

// input from vertex shader
in vec2 tc;

// the only output variable
out vec4 fragColor;

uniform sampler2D	TEX;
uniform vec2		texel_offset;
uniform int			pass_n;

const mat3 sobelFilter = mat3
(
	-1,-2,-1,
	0,0,0,
	1,2,1
);


void main()
{	
	mat3 grayTexels;
	vec3 texel = vec3(0);
	
	if(pass_n == 3)
	{
		fragColor = texture(TEX, tc);
	}

	for(int x=0; x<3; x++)
	{
		for(int y=0; y<3; y++)
		{
			vec2 offset = vec2(x-1, y-1) * texel_offset;
			texel = texture(TEX, tc + offset).rgb;
			grayTexels[x][y] = 0.299*texel.r + 0.507*texel.g + 0.114 * texel.b;
		}
	}

	if(pass_n == 1) 
	{
		fragColor = vec4(0.299*texel.r + 0.507*texel.g + 0.114 * texel.b);
		return;
	}
	
	if(pass_n == 2)
	{
		float Gx=0.0f;
		Gx += dot(sobelFilter[0].xyz, grayTexels[0].xyz);
		Gx += dot(sobelFilter[2].xyz, grayTexels[2].xyz);
	
		float Gy=0.0f;
		Gy += dot(sobelFilter[0].xyz, transpose(grayTexels)[0].xyz);
		Gy += dot(sobelFilter[2].xyz, transpose(grayTexels)[2].xyz);

		float G = sqrt(pow(Gx, 2)+pow(Gy,2));

		fragColor = vec4(vec3(G), 1.0f);
	}
}
