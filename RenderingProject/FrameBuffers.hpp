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
		glViewport(0, 0, resolution.x, resolution.y);
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
	unsigned int depthTex;
	const int octSize = 512;

	ShadowBuffer(unsigned int numLights) {
		glGenFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		glGenTextures(1, &depthTex);

		glBindTexture(GL_TEXTURE_2D, depthTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, octSize/2, octSize/2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTex, 0);

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glGenTextures(1, &octmapTex);

		glBindTexture(GL_TEXTURE_2D_ARRAY, octmapTex);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, octSize, octSize, numLights, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}

	void bindTexture(const int textureUnit) {
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, octmapTex);
	}

	void startWrite() {
		glViewport(0, 0, octSize/2, octSize/2);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);
	}

	void copyQuadrant(int quadrant, unsigned int layer) {
		const int quadSize = octSize / 2;
		int xoffset = 0, yoffset = 0;
		if (quadrant == 1) {
			xoffset = quadSize;
		}
		else if (quadrant == 2) {
			xoffset = quadSize;
			yoffset = quadSize;
		}
		else if (quadrant == 3) {
			yoffset = quadSize;
		}
		//glReadBuffer(GL_BACK);
		//glCopyTextureSubImage3D(octmapTex, 0, xoffset, yoffset, layer, 0, 0, octSize / 2, octSize / 2);
		//glReadBuffer(GL_NONE);

		auto depthPix = new float[quadSize * quadSize];
		glGetTextureSubImage(depthTex, 0, 0, 0, 0, quadSize, quadSize, 1, GL_DEPTH_COMPONENT, GL_FLOAT, sizeof(float) * quadSize * quadSize, depthPix);

		auto octPix = new float[quadSize * quadSize];
		glGetTextureSubImage(octmapTex, 0, xoffset, yoffset, layer, quadSize, quadSize, 1, GL_DEPTH_COMPONENT, GL_FLOAT, sizeof(float) * quadSize * quadSize, octPix);

		for (int i = 0; i < quadSize * quadSize; ++i) {
			if (!octPix[i]) octPix[i] = depthPix[i];
			else if(octPix[i] != depthPix[i]) octPix[i] = octPix[i] * depthPix[i];
		}

		glTextureSubImage3D(octmapTex, 0, xoffset, yoffset, layer, quadSize, quadSize, 1, GL_DEPTH_COMPONENT, GL_FLOAT, octPix);
	}
};