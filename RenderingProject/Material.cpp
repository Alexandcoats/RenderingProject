#include "Material.hpp"
#include <iostream>
#include <stb_image.h>

Material::Material(tinyobj::material_t material) : material(material) {
	loadTextures();
}

void Material::draw(int vaoID, int texLoc) {

	//glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[0]);
	glBindVertexBuffer(vaoID, bufferIDs[0], 0, sizeof(Vertex));

	auto texture = textures[material.diffuse_texname];
	if (texture) texture->draw(texLoc);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIDs[1]);
	glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, (void*)0);
}

void Material::createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data) {
	glGenBuffers(1, bufferID);
	glBindBuffer(GL_ARRAY_BUFFER, *bufferID);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Material::createIndexBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data) {
	glGenBuffers(1, bufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *bufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Material::bindVertexBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, bufferIDs[0]);
}

void Material::createBuffers() {
	//std::cout << "Creating model VBOs in OpenGL" << std::endl;

	//create vertex buffer
	createBuffer(&bufferIDs[0], vertices.size() * sizeof(Vertex), vertices.data());

	//create index buffer
	createIndexBuffer(&bufferIDs[1], indices.size() * sizeof(unsigned int), indices.data());

	indexSize = indices.size();

	vertices.clear();
	indices.clear();
}

void Material::loadTexture(std::string filepath) {
	int height, width, channels;
	unsigned char * pixels;

	auto texture = std::make_shared<Texture>(textures.size());
	pixels = stbi_load(filepath.c_str(), &width, &height, &channels, 4);
	texture->push(pixels, width, height, channels);
	textures[filepath] = texture;
}

void Material::loadTextures() {
	if (material.diffuse_texname != "") {
		loadTexture(material.diffuse_texname);
	}
	// Load the rest of the textures if we want them
}

Material::~Material() {
	//glDeleteBuffers(2, bufferIDs);
}