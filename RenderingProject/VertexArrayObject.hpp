#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <memory>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	bool operator==(const Vertex & other) const {
		return pos == other.pos && texCoord == other.texCoord;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

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

	void writeVertexAttribute(unsigned int location, int size, int offset, unsigned int bindingIndex);

	void divideVertexAttribute(unsigned int bindingIndex, unsigned int divisor);
};