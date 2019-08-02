#include "Shader.hpp"
#include <vector>
#include <fstream>

Shader::Shader(const char * filepath, unsigned int type): type(type)
{
	ID = glCreateShader(type);

	const std::string shaderSource = readShader(filepath);
	const char * source = shaderSource.c_str();
	glShaderSource(ID, 1, &source, nullptr);

	glCompileShader(ID);

#ifndef NDEBUG
	checkForErrors();
#endif // !NDEBUG

}

std::string Shader::readShader(const char* filepath) const {
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	if(file) return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	throw std::runtime_error("Failed to open shader file");
}

#ifndef NDEBUG
void Shader::checkForErrors() const {
	int success = 0;
	glGetShaderiv(ID, GL_COMPILE_STATUS, &success);

	if (success == GL_TRUE) return;

	int logSize = 0;
	glGetShaderiv(ID, GL_INFO_LOG_LENGTH, &logSize);

	std::vector<char> logInfo(logSize);
	glGetShaderInfoLog(ID, logSize, nullptr, &logInfo[0]);
	throw std::runtime_error(logInfo.data());
}
#endif // !NDEBUG



Shader::~Shader()
{
	//glDeleteShader(ID);
}
