#pragma once
#include "FrameBuffer.hpp"

class GBuffer : public FrameBuffer {
public:
	enum class Texture : uint32_t
	{
		Normal,
		Color,
		MSSR,
		SASS,
		CC,
		Depth
	};
	glm::ivec2 resolution;
	unsigned int renderTextures[6];

	GBuffer(const glm::ivec2 resolution) : resolution(resolution)
	{
		glGenFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glGenTextures(6, renderTextures);

		glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::Normal]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::Normal], 0);

		glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::Color]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::Color], 0);

		glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::MSSR]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::MSSR], 0);

		glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::SASS]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::SASS], 0);

		glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::CC]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, resolution.x, resolution.y, 0, GL_RG, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::CC], 0);

		glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::Depth]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, resolution.x, resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderTextures[(uint32_t)Texture::Depth], 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void copyToSystemFramebuffer() {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x, resolution.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void startWrite() {
		glBindFramebuffer(GL_FRAMEBUFFER, ID);
		unsigned int a[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
		glDrawBuffers(5, a);
	}

	void bindTexture(uint32_t tex, const int textureUnit) {
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, renderTextures[tex]);
	}
};

class ShadowBuffer : public FrameBuffer {
public:
	unsigned int octmapTex;
	const int octSize = 512;

	ShadowBuffer(int numLights) {
		glGenFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glGenTextures(1, &octmapTex);

		glBindTexture(GL_TEXTURE_2D_ARRAY, octmapTex);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB16F, octSize, octSize, numLights, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, octmapTex, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void bindTexture(const int textureUnit) {
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_3D, octmapTex);
	}

	void startWrite() {
		glBindFramebuffer(GL_FRAMEBUFFER, ID);
	}
};