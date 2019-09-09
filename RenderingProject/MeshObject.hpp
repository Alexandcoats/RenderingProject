#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <algorithm>
#include "VertexArrayObject.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <memory>
#include "Texture.hpp"
#include <tiny_obj_loader.h>
#include <unordered_map>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	bool operator==(const Vertex & other) const {
		return pos == other.pos && texCoord == other.texCoord;
	}
};

struct Material {
	tinyobj::material_t material;
	unsigned int bufferIDs[2];
	VertexArrayObject * vao;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int indexSize;
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
	int locations[4];

	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	std::vector<Material> materials;

	glm::vec3 pos;

	MeshObject(const int locations[4], std::vector<Material> materials);
	~MeshObject();

	// Disable copying of Meshes
	MeshObject(const MeshObject &) = delete;

	MeshObject& operator=(const MeshObject&) = delete;

	void createBuffers(Material & material);

	void loadTexture(std::string filepath);

	void loadTextures(const Material & material);

	void draw();

private:

	void createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
	void createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
};
