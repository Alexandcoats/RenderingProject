#pragma once
#include "Material.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

class SubMesh {
public:
	glm::vec3 location;
	glm::vec3 rotation;
	std::vector<std::shared_ptr<Material>> materials;

	SubMesh(std::vector<std::shared_ptr<Material>> materials, glm::vec3 location, glm::vec3 rotation) : materials(materials), location(location), rotation(rotation) {

	}

	void draw(glm::mat4 tileM, int vaoID, int mLoc, int nLoc, int texLoc) const {
		glm::mat4 m = tileM * glm::translate(glm::mat4(), location) * glm::rotate(glm::mat4(), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 n = glm::inverseTranspose(m);
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &m[0][0]);
		for (const auto & material : materials) {
			material->draw(vaoID, texLoc);
		}
	}
};

class Tile {
public:
	glm::vec3 location;
	std::vector<std::shared_ptr<Material>> materials;
	std::vector<SubMesh> subMeshes;

	Tile(std::vector<std::shared_ptr<Material>> materials, glm::vec3 location) : materials(materials), location(location) {

	}

	void draw(const Map::MinimalPiece * piece, glm::mat4 v, int vaoID, int mLoc, int nLoc, int texLoc, int xpos, int zpos) const {
		// First draw the tile
		glm::mat4 flip = glm::scale(glm::mat4(), glm::vec3(piece->flp ? -1.0f : 1.0f, 1.0f, 1.0f));
		glm::mat4 rotate = glm::rotate(glm::mat4(), (glm::pi<float>() / 2.0f) * piece->rot, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3((float)xpos * 20.0f, 0.0f, (float)zpos * 20.0f));
		glm::mat4 m = translate * rotate * flip;
		glm::mat4 n = glm::inverseTranspose(m);
		glUniformMatrix4fv(nLoc, 1, GL_FALSE, &n[0][0]);
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &m[0][0]);
		for (const auto & material : materials) {
			material->draw(vaoID, texLoc);
		}
		for (const auto & mesh : subMeshes) {
			// Next draw any sub-meshes it has
			mesh.draw(m, vaoID, mLoc, nLoc, texLoc);
		}
	}
};