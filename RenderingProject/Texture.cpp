#include "Texture.hpp"

Texture::Texture(unsigned int textureLocation, unsigned int textureUnit): textureLocation(textureLocation), textureUnit(textureUnit)
{
	if (textureUnit > GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) throw std::runtime_error("Too many textures have been created!");
	glGenTextures(1, &ID);
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture::bind() {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, ID);
	glUniform1i(textureLocation, textureUnit);
}

void Texture::push(unsigned char * pixels, int width, int height, int channels) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	int format;
	switch (channels) {
	case 1: 
		format = GL_R;
		break;
	case 2: 
		format = GL_RG;
		break;
	case 3: 
		format = GL_RGB;
		break;
	case 4: 
		format = GL_RGBA;
		break;
	default: 
		throw std::runtime_error("Invalid number of channels supplied for texture");
	}
	glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, pixels);
}


Texture::~Texture()
{
	//glDeleteTextures(1, &ID);
}
