#pragma once
#include <glad/glad.h>
#include <initializer_list>
#include <map>
#include <vector>
#include "Shader.hpp"
#include "VertexArrayObject.hpp"

class Pipeline
{
public:
	unsigned int ID;
	std::map<std::string, std::vector<int>> map;

	VertexArrayObject vao;

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

	virtual void createVAO()=0;

	int getAttributeLocation(std::string attribute);

#ifndef NDEBUG
	void checkForErrors();
#endif // !NDEBUG
	
};

