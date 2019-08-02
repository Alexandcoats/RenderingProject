#include "VertexArrayObject.hpp"
#include <glad/glad.h>

VertexArrayObject::VertexArrayObject() {
	glGenVertexArrays(1, &ID);
}

void VertexArrayObject::bind() {
	glBindVertexArray(ID);
}

void VertexArrayObject::writeVertexAttribute(unsigned int location, int size, int stride, const void * offset) {
	glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(location);
}

void VertexArrayObject::unbind() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArrayObject::~VertexArrayObject()
{
	glDeleteVertexArrays(1, &ID);
}
