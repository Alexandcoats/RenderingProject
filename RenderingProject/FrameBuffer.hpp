#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

class FrameBuffer
{
public:
	unsigned int ID;

	FrameBuffer() {};
	~FrameBuffer() {};

	void bind();

	virtual void startWrite() = 0;

	void endWrite();
};

