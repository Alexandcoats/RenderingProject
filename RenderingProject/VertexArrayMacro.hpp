#pragma once
#include "Geometry.hpp"

class VertexArrayMacro
{
public:

	unsigned int ID;
	unsigned int vertexBufID;

	VertexArrayMacro();

	VertexArrayMacro(int posLocation, int texCoordLocation, std::vector<Vertex> vertices);

	// Disable copying of VAOs
	VertexArrayMacro(const VertexArrayMacro &) = delete;

	VertexArrayMacro& operator=(const VertexArrayMacro&) = delete;

	~VertexArrayMacro();

	void bind();

	void startRecording();

	void stopRecording();

	void bindVertices(unsigned int posLocation, unsigned int texCoordLocation, std::vector<Vertex> vertices);
};

