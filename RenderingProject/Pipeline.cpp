#include "Pipeline.hpp"
#include <glm/gtc/matrix_transform.hpp>

Pipeline::Pipeline() : ID(glCreateProgram()) {}

Pipeline::Pipeline(const std::initializer_list<const Shader *> shaders) : ID(glCreateProgram())
{
	for (const Shader * shader : shaders) attachShader(shader);
	linkProgram();
	getAttributeData();
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

void Pipeline::attachShader(const Shader * shader) {
	glAttachShader(ID, shader->ID);
}

void Pipeline::draw(glm::mat4 vp, std::vector<std::vector<Map::MinimalPiece *>> pieces) {
	for (int i = 0; i < pieces.size(); ++i) {
		for (int j = 0; j < pieces[0].size(); ++j) {
			if (pieces[i][j]) {
				glm::mat4 mvp = vp * glm::translate(
										glm::rotate(
											glm::scale(
												glm::mat4(), 
												glm::vec3(1.0f, -1.0f, 1.0f * pieces[i][j]->flp)),
											(glm::pi<float>() / 2.0f) * pieces[i][j]->rot,
											glm::vec3(0.0f, 0.0f, 1.0f)), 
										glm::vec3((float)i * 2.0f, (float)j * 2.0f, 0.0f));
				glUniformMatrix4fv(map["mvp"][1], 1, GL_FALSE, &mvp[0][0]);
				meshes[pieces[i][j]->pieceInd - 1]->draw();
			}
		}
	}
	//for (auto mesh : meshes) {
	//	mesh.draw();
	//}
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
