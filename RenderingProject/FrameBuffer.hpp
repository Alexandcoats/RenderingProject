#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class FrameBuffer
{
public:
	unsigned int ID;
	unsigned int renderTextures[2];
	glm::ivec2 resolution;

	FrameBuffer(const glm::ivec2 resolution);
	~FrameBuffer();

	void startWrite();

	void endWrite();

	void bindColorTexture(const int textureUnit);
	void bindDepthTexture(const int textureUnit);

	void copyToSystemFramebuffer(const glm::ivec2 systemResolution, const unsigned int buffer, const int filter);

private:
	enum class Textures : uint32_t
	{
		Color,
		Depth
	};
};

