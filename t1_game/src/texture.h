#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "cgmath.h"
#include "cgut.h"
#include "stb_image.h"

struct Texture 
{
	std::vector<const char*> texture_path;
	std::vector<GLuint> texture;
	ivec2	window_size;

	void load(const char* texture_path) 
	{
		GLuint texture_;

		int width, height, comp = 3;
		unsigned char* pimage0 = stbi_load(texture_path, &width, &height, &comp, 3); if (comp == 1) comp = 3; /* convert 1-channel to 3-channel image */
		int stride0 = width * comp, stride1 = (stride0 + 3) & (~3);	// 4-byte aligned stride
		unsigned char* pimage = (unsigned char*)malloc(sizeof(unsigned char) * stride1 * height);
		for (int y = 0; y < height; y++) memcpy(pimage + (height - 1 - y) * stride1, pimage0 + y * stride0, stride0); // vertical flip

		// create textures
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8 /* GL_RGB for legacy GL */, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pimage);

		// allocate and create mipmap
		int mip_levels = miplevels(window_size.x, window_size.y);
		for (int k = 1, w = width >> 1, h = height >> 1; k < mip_levels; k++, w = max(1, w >> 1), h = max(1, h >> 1))
			glTexImage2D(GL_TEXTURE_2D, k, GL_RGB8 /* GL_RGB for legacy GL */, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);

		// configure texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		texture.push_back(texture_);

		// release the new image
		free(pimage);
	}
};

#endif
