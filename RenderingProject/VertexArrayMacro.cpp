#include "VertexArrayMacro.hpp"

VertexArrayMacro::VertexArrayMacro() {
	glGenVertexArrays(1, &ID);
}

void VertexArrayMacro::bind() {
	glBindVertexArray(ID);
}

void VertexArrayMacro::createBuffer(const char * name, GLsizeiptr size, const void * data) {
	unsigned int bufID;
	glGenBuffers(1, &bufID);
	glBindBuffer(GL_ARRAY_BUFFER, bufID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	bufferIDs[name] = bufID;
}

void VertexArrayMacro::bindVertexAttribute(const char * name, unsigned int location, int size, int stride, const void * offset) {
	glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[name]);

	glVertexAttribPointer(location, size, GL_FLOAT, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(location);
}

void VertexArrayMacro::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

VertexArrayMacro::~VertexArrayMacro()
{
	for(auto pair : bufferIDs) glDeleteBuffers(1, &pair.second);
	glDeleteVertexArrays(1, &ID);
}
