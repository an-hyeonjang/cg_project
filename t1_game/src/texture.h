#pragma once
#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "cgmath.h"
#include "cgut.h"

/********************************/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Texture 
{
	std::vector<const char*> path;
	std::vector<GLuint> texture;
	ivec2	window_size;

	void load(const char* texture_path)
	{
		GLuint texture_;

		int width, height, comp;
		unsigned char* pimage0 = stbi_load(texture_path, &width, &height, &comp, 4);
		int stride0 = width * comp, stride1 = (stride0 + 3) & (~3);	// 4-byte aligned stride
		unsigned char* pimage = (unsigned char*)malloc(sizeof(unsigned char) * stride1 * height);
		for (int y = 0; y < height; y++) memcpy(pimage + (height - 1 - y) * stride1, pimage0 + y * stride0, stride0); // vertical flip
		stbi_image_free(pimage0); // release the original image

		// create a particle texture
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pimage);


		// configure texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		texture.push_back(texture_);

		free(pimage);
	}

	void load_m(const char* texture_path) 
	{
		GLuint texture_;

		int width, height, comp;
		unsigned char* pimage0 = stbi_load(texture_path, &width, &height, &comp, 4); /* convert 1-channel to 3-channel image */
		int stride0 = width * comp, stride1 = (stride0 + 4) & (~4);	// 4-byte aligned stride
		unsigned char* pimage = (unsigned char*)malloc(sizeof(unsigned char) * stride1 * height);
		for (int y = 0; y < height; y++) memcpy(pimage + (height - 1 - y) * stride1, pimage0 + y * stride0, stride0); // vertical flip
		stbi_image_free(pimage0);

		// create textures
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA /* GL_RGB for legacy GL */, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pimage);

		// allocate and create mipmap
		int mip_levels = miplevels(window_size.x, window_size.y);
		for (int k = 1, w = width >> 1, h = height >> 1; k < mip_levels; k++, w = max(1, w >> 1), h = max(1, h >> 1))
			glTexImage2D(GL_TEXTURE_2D, k, GL_RGBA /* GL_RGB for legacy GL */, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glGenerateMipmap(GL_TEXTURE_2D);

		// configure texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		texture.push_back(texture_);

		// release the new image
		free(pimage);
	}
};

#endif
