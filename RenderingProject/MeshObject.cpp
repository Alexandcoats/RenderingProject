#include "MeshObject.hpp"
#include <iostream>
#include <stb_image.h>

MeshObject::MeshObject(const int locations[4], std::vector<Material> materials) : materials(materials) {
	this->locations[0] = locations[0];
	this->locations[1] = locations[1];
	this->locations[2] = locations[2];
	this->locations[3] = locations[3];

	for (auto & m : this->materials) {
		createBuffers(m);
		loadTextures(m);
	}
}

void MeshObject::draw() {
	for (const auto & m : materials) {
		auto texture = textures[m.material.diffuse_texname];
		if (texture) texture->draw();
		m.vao->bind();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.bufferIDs[1]);
		glDrawElements(GL_TRIANGLES, m.indexSize, GL_UNSIGNED_INT, (void*)0);
		m.vao->unbind();
	}
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


void MeshObject::createBuffers(Material & material) {
	//std::cout << "Creating model VBOs in OpenGL" << std::endl;

	material.vao = new VertexArrayObject();
	material.vao->bind();

	//create vertex buffer
	createBuffer(&material.bufferIDs[0], material.vertices.size() * sizeof(Vertex), material.vertices.data());

	//create index buffer
	createIndexBuffer(&material.bufferIDs[1], material.indices.size() * sizeof(unsigned int), material.indices.data());

	material.vao->writeVertexAttribute(locations[0], 3, sizeof(Vertex), (void*)offsetof(Vertex, pos));

	if (locations[1] != -1)
	{
		material.vao->writeVertexAttribute(locations[1], 3, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	}

	if (locations[2] != -1)
	{
		material.vao->writeVertexAttribute(locations[2], 2, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	}


	material.vao->unbind();

	material.indexSize = material.indices.size();

	material.vertices.clear();
	material.indices.clear();
}

void MeshObject::loadTexture(std::string filepath) {
	int height, width, channels;
	unsigned char * pixels;

	auto texture = std::make_shared<Texture>(locations[3], textures.size());
	pixels = stbi_load(filepath.c_str(), &width, &height, &channels, 4);
	texture->push(pixels, width, height, channels);
	textures[filepath] = texture;
}

void MeshObject::loadTextures(const Material & material) {
	if (material.material.diffuse_texname != "") {
		loadTexture(material.material.diffuse_texname);
	}
	// Load the rest of the textures if we want them
}

MeshObject::~MeshObject() {
	//glDeleteBuffers(2, bufferIDs);
}