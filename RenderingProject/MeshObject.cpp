#define TINYOBJLOADER_IMPLEMENTATION
#include "MeshObject.hpp"
#include <iostream>

MeshObject::MeshObject(std::string filepath, int vertexLocation, int normalLocation, int uvLocation) : filepath(filepath) {
	locations[0] = vertexLocation;
	locations[1] = normalLocation;
	locations[2] = uvLocation;
	std::string extension = filepath.substr(filepath.size() - 4, 4);
	//read file
	if (extension == ".obj")
	{
		readOBJ(filepath);
	}
	/*
	else if (extension == ".pxo")
	{
		readPXO(filepath);
	}
	*/
	else
	{
		throw std::runtime_error("Cannot load file: invalid file path " + filepath);
	}
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

void MeshObject::readOBJ(std::string filepath) {
	//std::cout <<"Reading OBJ file: " << filepath << std::endl;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		throw std::runtime_error(warn + err);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	std::unordered_map<Vertex, unsigned int> uniqueVertices = {};

	for (const auto & shape : shapes) {
		for (const auto & index : shape.mesh.indices) {
			Vertex vertex = {};
			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (locations[1] != -1) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (locations[2] != -1) {
				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<unsigned int>(vertices.size());
				vertices.push_back(vertex);
			}
			indices.push_back(uniqueVertices[vertex]);
		}
	}

	//pass data to OpenGL
	this->createBuffers(vertices, indices);
}

/*

void MeshObject::readPXO(std::string filepath) {
	//std::cout << "Reading PXO file: " << filepath << std::endl;

	std::ifstream inFile(filepath, std::ios::binary);

	//get container sizes
	size_t vertSize, normSize, uvSize, indexSize;
	inFile.read(reinterpret_cast<char*>(&vertSize), sizeof(size_t));
	inFile.read(reinterpret_cast<char*>(&normSize), sizeof(size_t));
	inFile.read(reinterpret_cast<char*>(&uvSize), sizeof(size_t));
	inFile.read(reinterpret_cast<char*>(&indexSize), sizeof(size_t));

	//fetch data
	std::vector<float> verts(vertSize / sizeof(float));
	std::vector<float> norms(normSize / sizeof(float));
	std::vector<float> uvs(uvSize / sizeof(float));
	std::vector<unsigned int> indexes(indexSize / sizeof(unsigned int));

	inFile.read(reinterpret_cast<char*>(&verts.front()), vertSize);
	if (normSize > 0) inFile.read(reinterpret_cast<char*>(&norms.front()), normSize);
	if (uvSize > 0) inFile.read(reinterpret_cast<char*>(&uvs.front()), uvSize);
	inFile.read(reinterpret_cast<char*>(&indexes.front()), indexSize);

	this->createBuffers(verts, norms, uvs, indexes);
}

*/

MeshObject::~MeshObject() {
	//glDeleteBuffers(2, bufferIDs);
}