#pragma once
#include <glad/glad.h>
#include <vector>

class Texture
{
public:
	unsigned int ID;
	unsigned int textureLocation;
	unsigned int textureUnit;

	Texture(unsigned int textureLocation, unsigned int textureUnit);
	~Texture();

	// Disable copying of Textures
	Texture(const Texture &) = delete;

	Texture& operator=(const Texture&) = delete;

	void bind();

	void push(unsigned char * pixels, int width, int height);
};

