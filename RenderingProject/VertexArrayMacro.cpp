#include "VertexArrayMacro.hpp"
#include <glad/glad.h>

VertexArrayMacro::VertexArrayMacro() {
	glGenVertexArrays(1, &ID);
}

void VertexArrayMacro::bind() {
	glBindVertexArray(ID);
}

void VertexArrayMacro::writeVertexAttribute(unsigned int bufferID, unsigned int location, int size, int stride, const void * offset) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferID);

	glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(location);
}

void VertexArrayMacro::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

VertexArrayMacro::~VertexArrayMacro()
{
	glDeleteVertexArrays(1, &ID);
}
