#pragma once
#include "Geometry.hpp"
#include <map>

class VertexArrayMacro
{
public:

	unsigned int ID;
	std::map<const char*, unsigned int> bufferIDs;

	VertexArrayMacro();

	// Disable copying of VAOs
	VertexArrayMacro(const VertexArrayMacro &) = delete;

	VertexArrayMacro& operator=(const VertexArrayMacro&) = delete;

	~VertexArrayMacro();

	void bind();

	void unbind();

	void createBuffer(const char * name, GLsizeiptr size, const void * data);

	void bindVertexAttribute(const char * name, unsigned int location, int size, int stride, const void * offset);
};

