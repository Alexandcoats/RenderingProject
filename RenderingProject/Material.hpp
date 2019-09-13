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

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
					(hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^
					(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

class Material
{
public:
	tinyobj::material_t material;
	unsigned int bufferIDs[2];
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int indexSize;

	std::unordered_map<std::string, std::shared_ptr<Texture>> textures;

	glm::vec3 pos;

	Material(tinyobj::material_t material);
	~Material();

	// Disable copying of Meshes
	Material(const Material &) = delete;

	Material& operator=(const Material&) = delete;

	void createBuffers();

	void bindVertexBuffer();

	void loadTexture(std::string filepath);

	void loadTextures();

	void draw(int vaoID, std::function<int(std::string)> attrLocation);

private:

	void createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
	void createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
};
