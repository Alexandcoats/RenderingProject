#include "FrameBuffer.hpp"

FrameBuffer::FrameBuffer(const glm::ivec2 resolution): ID(0), resolution(resolution)
{
	glGenFramebuffers(1, &ID);
	glBindFramebuffer(GL_FRAMEBUFFER, ID);

	glGenTextures(2, renderTextures);

	glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Textures::Color]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, resolution.x, resolution.y);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTextures[(uint32_t)Textures::Color], 0);

	glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Textures::Depth]);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, resolution.x, resolution.y);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderTextures[(uint32_t)Textures::Depth], 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::startWrite() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glViewport(0, 0, resolution.x, resolution.y);

	static const unsigned int uintZeros[] = { 0, 0, 0, 0 };
	static const float floatOnes[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClearBufferuiv(GL_COLOR, 0, uintZeros);
	glClearBufferfv(GL_DEPTH, 0, floatOnes);
}

void FrameBuffer::endWrite() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
}

void FrameBuffer::bindColorTexture(const int textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Textures::Color]);
}

void FrameBuffer::bindDepthTexture(const int textureUnit) {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, renderTextures[(uint32_t)Textures::Depth]);
}

void FrameBuffer::copyToSystemFramebuffer(const glm::ivec2 systemResolution, const unsigned int buffer, const int filter) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glViewport(0, 0, systemResolution.x, systemResolution.y);
	glClear(buffer);

	glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x, resolution.y, buffer, filter);
}


FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &ID);
	glDeleteTextures(2, renderTextures);
}
