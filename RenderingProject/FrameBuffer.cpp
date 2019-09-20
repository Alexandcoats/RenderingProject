#include "FrameBuffer.hpp"

void FrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FrameBuffer::endWrite() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_BACK);
}
