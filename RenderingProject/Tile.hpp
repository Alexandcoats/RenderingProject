#pragma once
#include "MeshObject.hpp"
#include <glm/gtc/matrix_transform.hpp>

class SubMesh {
public:
	glm::vec3 location;
	glm::vec3 rotation;
	std::shared_ptr<MeshObject> tileMesh;

	SubMesh(std::shared_ptr<MeshObject> mesh, glm::vec3 location, glm::vec3 rotation) : tileMesh(mesh), location(location), rotation(rotation) {}

	void draw(glm::mat4 tileM, glm::mat4 tileFR, int mLoc, int frLoc) const {
		glm::mat4 fr = tileFR * glm::rotate(glm::mat4(), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 m = tileM * glm::translate(glm::mat4(), location) * glm::rotate(glm::mat4(), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::rotate(glm::mat4(), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(frLoc, 1, GL_FALSE, &fr[0][0]);
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &m[0][0]);
		tileMesh->draw();
	}
};

class Tile {
public:
	glm::vec3 location;
	std::unique_ptr<MeshObject> tileMesh;
	std::vector<SubMesh> subMeshes;

	Tile(std::unique_ptr<MeshObject> mesh, glm::vec3 location) : tileMesh(std::move(mesh)), location(location) {}

	void draw(const Map::MinimalPiece * piece, int mLoc, int frLoc, int xpos, int zpos) const {
		// First draw the tile
		glm::mat4 flip = glm::scale(glm::mat4(), glm::vec3(piece->flp ? -1.0f : 1.0f, 1.0f, 1.0f));
		glm::mat4 rotate = glm::rotate(glm::mat4(), (glm::pi<float>() / 2.0f) * piece->rot, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 translate = glm::translate(glm::mat4(), glm::vec3((float)xpos * 20.0f, 0.0f, (float)zpos * 20.0f));
		glm::mat4 fr = rotate * flip;
		glm::mat4 m = translate * fr;
		glUniformMatrix4fv(frLoc, 1, GL_FALSE, &fr[0][0]);
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, &m[0][0]);
		tileMesh->draw();
		for (const auto & mesh : subMeshes) {
			// Next draw any sub-meshes it has
			mesh.draw(m, fr, mLoc, frLoc);
		}
	}
};