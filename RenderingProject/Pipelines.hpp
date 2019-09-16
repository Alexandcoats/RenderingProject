#pragma once
#include "Pipeline.hpp"

class GeometryPipeline : public Pipeline {
public:
	std::vector<Tile> tiles;

	GeometryPipeline(const std::initializer_list<const Shader *> shaders) : Pipeline(shaders) {
		createVAO();
	}

	void draw(std::vector<std::vector<Map::MinimalPiece *>> pieces) {
		vao.bind();
		for (int i = 0; i < pieces.size(); ++i) {
			for (int j = 0; j < pieces[0].size(); ++j) {
				if (pieces[i][j]->pieceInd) {
					tiles[pieces[i][j]->pieceInd - 1].draw(pieces[i][j], vao.ID, [=](std::string attribute) {return this->getAttributeLocation(attribute); }, j, i);
				}
			}
		}
		vao.unbind();
	}

	void createVAO() {
		vao.bind();

		vao.writeVertexAttribute(getAttributeLocation("pos"), 3, offsetof(Vertex, pos));

		vao.writeVertexAttribute(getAttributeLocation("normal"), 3, offsetof(Vertex, normal));

		vao.writeVertexAttribute(getAttributeLocation("texCoord"), 2, offsetof(Vertex, texCoord));

		vao.unbind();
	}

};

class DeferredPipeline : public Pipeline {
public:
	struct LightVertex {
		glm::vec2 pos;
		glm::vec2 texCoord;
	};
	std::vector<LightVertex> vertices = {
		{ {-1,1}, {0,1} },
		{ {-1,-1}, {0,0} },
		{ {1,-1}, {1,0} },
		{ {1,1}, {1,1} }
	};
	std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

	unsigned int bufferIDs[2];

	DeferredPipeline(const std::initializer_list<const Shader *> shaders) : Pipeline(shaders) {
		glGenBuffers(1, &bufferIDs[0]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[0]);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LightVertex) , vertices.data(), GL_STATIC_DRAW);

		glGenBuffers(1, &bufferIDs[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		createVAO();
	}

	void draw() {
		vao.bind();
		glBindVertexBuffer(vao.ID, bufferIDs[0], 0, sizeof(LightVertex));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[1]);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
		vao.unbind();
	}

	void createVAO() {
		vao.bind();

		vao.writeVertexAttribute(getAttributeLocation("pos"), 2, offsetof(LightVertex, pos));

		vao.writeVertexAttribute(getAttributeLocation("texCoord"), 2, offsetof(LightVertex, texCoord));

		vao.unbind();
	}
};