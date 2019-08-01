#pragma once

#include <algorithm>
#include <unordered_map>
#include <glad/glad.h>
#include <glm\glm.hpp>
#include <tiny_obj_loader.h>
#include "VertexArrayMacro.hpp"

/*
  Resource type for loading vertex geometry data.
  Data is stored into OpenGL in the form of mutliple attribute arrays

  Currently supports:
  vertex(location 0)
  normal(location 1)
  UV(location 2)
  index
*/

class MeshObject
{
public:
	unsigned int bufferIDs[4];
	unsigned int locations[3];

	VertexArrayMacro * vertexMacro, * normalMacro, * uvMacro;

	unsigned int indexSize;

	std::string filepath;

	MeshObject(std::string filepath, unsigned int vertexLocation, unsigned int normalLocation, unsigned int uvLocation);
	~MeshObject();

	void createBuffers(std::vector<float> vertexData, std::vector<float> normalData, std::vector<float> uvData, std::vector<unsigned int> indexData);

	void writeMacros();

	void draw();

private:

	void readOBJ(std::string filepath);
	void readPXO(std::string filepath);
	void createBuffer(unsigned int * bufferID, GLsizeiptr size, const void * data);
};
