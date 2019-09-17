#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <tiny_obj_loader.h>
#include <unordered_map>
#include "Pipelines.hpp"
#include "Texture.hpp"

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

	void draw(GeometryPipeline * pipeline, unsigned int numInstances, unsigned int instanceVBO);

private:

	void createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
	void createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
};
