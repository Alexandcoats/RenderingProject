#include "VertexArrayMacro.hpp"

VertexArrayMacro::VertexArrayMacro() {

}

VertexArrayMacro::VertexArrayMacro(int posLocation, int texCoordLocation, std::vector<Vertex> vertices)
{
	startRecording();
	bindVertices(posLocation, texCoordLocation, vertices);
	stopRecording();
}

void VertexArrayMacro::bind() {
	glBindVertexArray(ID);
}

void VertexArrayMacro::startRecording() {
	glGenVertexArrays(1, &ID);
	bind();
}

void VertexArrayMacro::bindVertices(unsigned int posLocation, unsigned int texCoordLocation, std::vector<Vertex> vertices) {
	glGenBuffers(1, &vertexBufID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 2, (void*)0);
	glEnableVertexAttribArray(posLocation);

	glVertexAttribPointer(texCoordLocation, 3, GL_FLOAT, GL_FALSE, 3, (void*)3);
	glEnableVertexAttribArray(texCoordLocation);
}

void VertexArrayMacro::stopRecording() {
	glBindVertexArray(0);
}

VertexArrayMacro::~VertexArrayMacro()
{
	glDeleteBuffers(1, &vertexBufID);
	glDeleteVertexArrays(1, &ID);
}
