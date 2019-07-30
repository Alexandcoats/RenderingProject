#pragma once

class VertexArrayMacro
{
public:

	unsigned int ID;

	VertexArrayMacro();

	// Disable copying of VAOs
	VertexArrayMacro(const VertexArrayMacro &) = delete;

	VertexArrayMacro& operator=(const VertexArrayMacro&) = delete;

	~VertexArrayMacro();

	void bind();

	void unbind();

	void writeVertexAttribute(unsigned int bufferID, unsigned int location, int size, int stride, const void * offset);
};

