#define STB_IMAGE_IMPLEMENTATION

#include "TextureObject.h"

void TextureObject::bindResource()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
}

void TextureObject::freeResource()
{
}

void TextureObject::deleteResource()
{
	glDeleteTextures(1, &texId);
}

TextureObject::TextureObject(std::string filepath) : ObjectResource(filepath)
{
	std::cout << "Reading texture file: " << filepath << std::endl;

	//find working directory
	std::string fullPath = getWorkingDir() + filepath;

	//fetch image data
	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		throw std::runtime_error("Error loading file: " + filepath);
	}

	//load data into OpenGL
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	//wrapping params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//filtering params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

TextureObject::~TextureObject()
{
	deleteResource();
}

GLuint TextureObject::getTexId()
{
	return texId;
}
