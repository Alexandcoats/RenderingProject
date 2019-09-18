#include "VertexArrayObject.hpp"
#include <glad/glad.h>

VertexArrayObject::VertexArrayObject() {
	glCreateVertexArrays(1, &ID);
}

void VertexArrayObject::bind() {
	glBindVertexArray(ID);
}

void VertexArrayObject::writeVertexAttribute(unsigned int location, int size, int offset, unsigned int bindingIndex) {
	glEnableVertexArrayAttrib(ID, location);
	glVertexArrayAttribFormat(ID, location, size, GL_FLOAT, GL_FALSE, offset);
	glVertexArrayAttribBinding(ID, location, bindingIndex);
}

void VertexArrayObject::divideVertexAttribute(unsigned int bindingIndex, unsigned int divisor) {
	glVertexArrayBindingDivisor(ID, bindingIndex, divisor);
}

void VertexArrayObject::unbind() {
	glBindVertexArray(0);
}

VertexArrayObject::~VertexArrayObject()
{
	//glDeleteVertexArrays(1, &ID);
}
