#define TINYOBJLOADER_IMPLEMENTATION
#include "MeshObject.hpp"
#include <iostream>

MeshObject::MeshObject(const int locations[3], std::vector<Vertex> vertexData, std::vector<unsigned int> indexData) {
	this->locations[0] = locations[0];
	this->locations[1] = locations[1];
	this->locations[2] = locations[2];

	createBuffers(vertexData, indexData);
}

void MeshObject::draw() {
	texture->draw();
	vao->bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[1]);
	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, (void*)0);
	vao->unbind();
}

void MeshObject::createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data) {
	glGenBuffers(1, bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, *bufferID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void MeshObject::createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data) {
	glGenBuffers(1, bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


void MeshObject::createBuffers(std::vector<Vertex> vertexData, std::vector<unsigned int> indexData) {
	//std::cout << "Creating model VBOs in OpenGL" << std::endl;

	vao = new VertexArrayObject();
	vao->bind();

	//create vertex buffer
	createBuffer(&bufferIDs[0], vertexData.size() * sizeof(Vertex), vertexData.data());

	//create index buffer
	createIndexBuffer(&bufferIDs[1], indexData.size() * sizeof(unsigned int), indexData.data());

	vao->writeVertexAttribute(locations[0], 3, sizeof(Vertex), (void*)offsetof(Vertex, pos));

	if (locations[1] != -1)
	{
		vao->writeVertexAttribute(locations[1], 3, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	}

	if (locations[2] != -1)
	{
		vao->writeVertexAttribute(locations[2], 2, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	}


	vao->unbind();

	indexSize = indexData.size();
}

MeshObject::~MeshObject() {
	//glDeleteBuffers(2, bufferIDs);
}