#include "Pipeline.hpp"
#include <algorithm>
#include <vector>

Pipeline::Pipeline() : ID(glCreateProgram()) {}

Pipeline::Pipeline(const std::initializer_list<const Shader *> shaders) : ID(glCreateProgram()) 
{
	for (const Shader * shader : shaders) attachShader(shader);
	linkProgram();
}

void Pipeline::linkProgram() {
	glLinkProgram(ID);

	int shaderCount;
	glGetProgramiv(ID, GL_ATTACHED_SHADERS, &shaderCount);

	std::vector<unsigned int> attachedShaders(shaderCount);
	glGetAttachedShaders(ID, shaderCount, nullptr, &attachedShaders[0]);

	for (int shaderID : attachedShaders) {
		glDetachShader(ID, shaderID);
	}

#ifndef NDEBUG
	checkForErrors();
#endif // !NDEBUG

}

void Pipeline::attachShader(const Shader * shader) {
	glAttachShader(ID, shader->ID);
}

#ifndef NDEBUG
void Pipeline::checkForErrors() {
	int success = 0;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);

	if (success == GL_TRUE) return;

	int logSize = 0;
	glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &logSize);

	std::vector<char> logInfo(logSize);
	glGetProgramInfoLog(ID, logSize, nullptr, &logInfo[0]);
	throw std::runtime_error(logInfo.data());
}
#endif // !NDEBUG


Pipeline::~Pipeline()
{
	glDeleteProgram(ID);
}
