#pragma once

#include <algorithm>
#include <unordered_map>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <tiny_obj_loader.h>
#include "VertexArrayObject.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "Texture.hpp"

/*
  Resource type for loading vertex geometry data.
  Data is stored into OpenGL in the form of mutliple attribute arrays

  Currently supports:
  vertex(location 0)
  normal(location 1)
  UV(location 2)
  index
*/

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

	VertexArrayObject * vao;

	unsigned int indexSize;

	std::string filepath;

	MeshObject(std::string filepath, int vertexLocation, int normalLocation, int uvLocation);
	~MeshObject();

	void createBuffers(std::vector<Vertex> vertexData, std::vector<unsigned int> indexData);

	void draw();

private:

	void readOBJ(std::string filepath);
	void readPXO(std::string filepath);
	void createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
	void createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
};
