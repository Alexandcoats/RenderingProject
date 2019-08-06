#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

class Shader
{
public:
	unsigned int ID;
	unsigned int type;

	Shader(const char * filepath, unsigned int type);

	// Disable copying of shaders
	Shader(const Shader &) = delete;

	Shader& operator=(const Shader&) = delete;

	~Shader();

#ifndef NDEBUG
	void checkForErrors() const;
#endif // !NDEBUG


private:
	std::string readShader(const char * filepath) const;
};

