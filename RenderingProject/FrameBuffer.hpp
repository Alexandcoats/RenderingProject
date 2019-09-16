#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class FrameBuffer
{
public:
	enum class Texture : uint32_t
	{
		Position,
		Normal,
		Color,
		MSSR,
		SASS,
		CC,
		Depth
	};
	unsigned int ID;
	unsigned int renderTextures[7];
	glm::ivec2 resolution;

	FrameBuffer(const glm::ivec2 resolution);
	~FrameBuffer();

	void bind();

	void startWrite();

	void endWrite();

	void bindTexture(Texture tex, const int textureUnit);

	void copyToSystemFramebuffer();
};

