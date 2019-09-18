#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Material.hpp"

struct Light {
	glm::vec3 pos;
	glm::vec3 color;
	float brightness;
};

class SubMesh {
public:
	glm::vec3 location;
	glm::vec3 rotation;
	std::vector<std::shared_ptr<Material>> materials;

	SubMesh(std::vector<std::shared_ptr<Material>> materials, glm::vec3 location, glm::vec3 rotation) : materials(materials), location(location), rotation(rotation) {

	}

	void draw(GeometryPipeline * pipeline, unsigned int numInstances, unsigned int instanceVBO) const {
		glm::mat4 m = glm::translate(glm::mat4(), location) * glm::rotate(glm::mat4(), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(pipeline->getAttributeLocation("subM"), 1, GL_FALSE, &m[0][0]);
		for (const auto & material : materials) {
			material->draw(pipeline, numInstances, instanceVBO);
		}
	}
};

class Tile {
public:
	std::vector<std::shared_ptr<Material>> materials;
	
	unsigned int instanceVBO;
	unsigned int numInstances;
	std::vector<glm::mat4> instances;
	std::vector<SubMesh> subMeshes;
	std::vector<Light> lights;

	Tile(std::vector<std::shared_ptr<Material>> materials) : materials(materials) {
		
	}

	void setInstances() {
		numInstances = instances.size();
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instances.size(), instances.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		instances.clear();
	}

	void draw(GeometryPipeline * pipeline) const {
		if (numInstances == 0) return;
		glUniformMatrix4fv(pipeline->getAttributeLocation("subM"), 1, GL_FALSE, &glm::mat4()[0][0]);
		for (const auto & material : materials) {
			material->draw(pipeline, numInstances, instanceVBO);
		}
		for (const auto & mesh : subMeshes) {
			// Next draw any sub-meshes it has
			mesh.draw(pipeline, numInstances, instanceVBO);
		}
	}
};