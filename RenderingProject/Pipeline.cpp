#include "Pipeline.hpp"

Pipeline::Pipeline() : ID(glCreateProgram()) {}

Pipeline::Pipeline(Geometry geometry, const std::initializer_list<const Shader *> shaders) : geometry(geometry), ID(glCreateProgram())
{
	for (const Shader * shader : shaders) attachShader(shader);
	linkProgram();
	getAttributeData();
	macro = new VertexArrayMacro(map["pos"][0], map["texCoord"][0], geometry.vertices);
	bindIndices(geometry.indices);
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

void Pipeline::getAttributeData() {
	int numActiveAttribs = 0;
	int numActiveUniforms = 0;
	glGetProgramInterfaceiv(ID, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &numActiveAttribs);
	glGetProgramInterfaceiv(ID, GL_UNIFORM, GL_ACTIVE_RESOURCES, &numActiveUniforms);

	std::vector<char> nameData(256);
	std::vector<unsigned int> properties;
	properties.push_back(GL_NAME_LENGTH);
	properties.push_back(GL_TYPE);
	properties.push_back(GL_LOCATION);
	std::vector<int> values(properties.size());
	for (int attrib = 0; attrib < numActiveAttribs; ++attrib)
	{
		glGetProgramResourceiv(ID, GL_PROGRAM_INPUT, attrib, properties.size(),
			&properties[0], values.size(), NULL, &values[0]);

		nameData.resize(values[0]); //The length of the name.
		glGetProgramResourceName(ID, GL_PROGRAM_INPUT, attrib, nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0], nameData.size() - 1);
		map[name] = { GL_PROGRAM_INPUT, values[2], values[1] };
	}

	for (int uniform = 0; uniform < numActiveUniforms; ++uniform)
	{
		glGetProgramResourceiv(ID, GL_UNIFORM, uniform, properties.size(),
			&properties[0], values.size(), NULL, &values[0]);

		nameData.resize(values[0]); //The length of the name.
		glGetProgramResourceName(ID, GL_UNIFORM, uniform, nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0], nameData.size() - 1);
		map[name] = { GL_UNIFORM, values[2], values[1] };
	}
}

void Pipeline::bindIndices(std::vector<uint16_t> indices) {
	glGenBuffers(1, &indexBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
}

void Pipeline::attachShader(const Shader * shader) {
	glAttachShader(ID, shader->ID);
}

void Pipeline::draw() {
	use();
	macro->bind();
	glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_SHORT, (void*)0);
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
	glDeleteBuffers(1, &indexBufID);
	glDeleteProgram(ID);
}
