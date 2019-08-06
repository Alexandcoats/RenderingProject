#pragma once
#include <glad/glad.h>
#include <initializer_list>
#include <map>
#include <vector>
#include "Shader.hpp"
#include "VertexArrayObject.hpp"
#include <vector>
#include "MeshObject.hpp"
#include "Map.hpp"

class Pipeline
{
public:
	unsigned int ID;
	std::map<std::string, std::vector<int>> map;
	std::vector<std::unique_ptr<MeshObject>> meshes;

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

	void draw(glm::mat4 vp, std::vector<std::vector<Map::MinimalPiece *>> map);

#ifndef NDEBUG
	void checkForErrors();
#endif // !NDEBUG
	
};

