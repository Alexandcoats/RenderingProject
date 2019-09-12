#pragma once

class VertexArrayObject
{
public:

	unsigned int ID;

	VertexArrayObject();

	// Disable copying of VAOs
	VertexArrayObject(const VertexArrayObject &) = delete;

	VertexArrayObject& operator=(const VertexArrayObject&) = delete;

	~VertexArrayObject();

	void bind();

	void unbind();

	void writeVertexAttribute(unsigned int location, int size, int offset);
};

