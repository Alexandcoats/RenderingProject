#pragma once

#include <algorithm>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VertexArrayObject.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "Texture.hpp"

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

class MeshObject
{
public:
	unsigned int bufferIDs[2];
	int locations[3];

	Texture * texture;

	glm::vec3 pos;

	VertexArrayObject * vao;

	unsigned int indexSize;

	MeshObject(const int locations[3], std::vector<Vertex> vertexData, std::vector<unsigned int> indexData);
	~MeshObject();

	// Disable copying of Meshes
	MeshObject(const MeshObject &) = delete;

	MeshObject& operator=(const MeshObject&) = delete;

	void createBuffers(std::vector<Vertex> vertexData, std::vector<unsigned int> indexData);

	void draw();

private:

	void createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
	void createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
};
