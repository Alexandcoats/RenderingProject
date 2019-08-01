#pragma once
#include <glad/glad.h>
#include <initializer_list>
#include <map>
#include <vector>
#include "Shader.hpp"
#include "VertexArrayMacro.hpp"
#include "Geometry.hpp"
#include <vector>
#include "MeshObject.hpp"

class Pipeline
{
public:
	unsigned int ID;
	unsigned int indexBufID;
	std::map<std::string, std::vector<int>> map;
	std::vector<MeshObject> meshes;

	Geometry geometry;

	Pipeline();

	explicit Pipeline(const std::initializer_list<const Shader*> shaders);

	// Disable copying of Pipelines
	Pipeline(const Pipeline &) = delete;

	Pipeline& operator=(const Pipeline&) = delete;

	~Pipeline();

	void linkProgram();

	void use() { glUseProgram(ID); };

	void attachShader(const Shader * shader);

	void getAttributeData();

	void draw();

#ifndef NDEBUG
	void checkForErrors();
#endif // !NDEBUG
	
};

