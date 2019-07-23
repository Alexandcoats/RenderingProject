#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <initializer_list>
#include "Shader.hpp"

class Pipeline
{
public:
	unsigned int ID;

	Pipeline();

	explicit Pipeline(const std::initializer_list<const Shader*> shaders);

	~Pipeline();

	void linkProgram();

	void use() { glUseProgram(ID); };

	void attachShader(const Shader * shader);

#ifndef NDEBUG
	void checkForErrors();
#endif // !NDEBUG
	
};

