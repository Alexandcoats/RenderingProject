#include "ShaderObject.h"

ShaderObject::ShaderObject(std::string input) : ObjectResource(input)
{
	//create shaders
	vertShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	//find working directory
	std::string dir = getWorkingDir();

	//std::cout << "working directory: " << dir << std::endl;

	//separate input into filepaths
	int delim = input.find_first_of('|');
	std::string vertFile = input.substr(0, delim);
	std::string fragFile = input.substr(delim + 1, input.size());

	// read vertex shader from file
	std::string vertCode;
	std::ifstream vertStream(dir + vertFile, std::ios::in);
	if (vertStream.is_open())
	{
		vertCode = std::string(std::istreambuf_iterator<char>(vertStream), std::istreambuf_iterator<char>());
		vertStream.close();
		//std::cout << vertCode << std::endl;
	}
	else
	{
		throw std::runtime_error("unable to open the vertex shader file");
	}

	// read fragment shader from file
	std::string fragCode;
	std::ifstream fragStream(dir + fragFile, std::ios::in);
	if (fragStream.is_open())
	{
		fragCode = std::string(std::istreambuf_iterator<char>(fragStream), std::istreambuf_iterator<char>());
		fragStream.close();
		//std::cout << fragCode << std::endl;
	}
	else
	{
		throw std::runtime_error("unable to open the fragment shader file");
	}

	// compile vertex shader
	const char* vertPointer = vertCode.c_str();
	glShaderSource(vertShaderID, 1, &vertPointer, NULL);
	glCompileShader(vertShaderID);
	GLint vertStatus;
	glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &vertStatus);
	if (vertStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(vertShaderID, infoLogLength, NULL, strInfoLog);

		std::string logStr = strInfoLog;
		delete[] strInfoLog;

		throw std::runtime_error("Compile failure in vertex shader: " + logStr);
	}

	// compile fragment shader
	const char* fragPointer = fragCode.c_str();
	glShaderSource(fragShaderID, 1, &fragPointer, NULL);
	glCompileShader(fragShaderID);
	GLint fragStatus;
	glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &fragStatus);
	if (fragStatus == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(fragShaderID, infoLogLength, NULL, strInfoLog);

		std::string logStr = strInfoLog;
		delete[] strInfoLog;

		throw std::runtime_error("Compile failure in fragment shader: " + logStr);
	}

	// add shaders to program
	programID = glCreateProgram();
	glAttachShader(programID, vertShaderID);
	glAttachShader(programID, fragShaderID);

	// link program to GL
	glLinkProgram(programID);
	GLint status;
	glGetProgramiv(programID, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(programID, infoLogLength, NULL, strInfoLog);
		std::string logStr = strInfoLog;
		delete[] strInfoLog;

		throw std::runtime_error("Linker error: " + logStr);
	}

	glDetachShader(programID, vertShaderID);
	glDetachShader(programID, fragShaderID);

	glDeleteShader(vertShaderID);
	glDeleteShader(fragShaderID);

	std::cout << "Shaders Loaded" << std::endl;
}

void ShaderObject::setPersective(bool input)
{
	usePerspective = input;
}

bool ShaderObject::isPerspective()
{
	return usePerspective;
}

void ShaderObject::use()
{
	glUseProgram(programID);
}

void ShaderObject::setBool(const std::string name, bool input)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)input);
}

void ShaderObject::setInt(const std::string name, int input)
{
	glUniform1i(glGetUniformLocation(programID, name.c_str()), input);
}

void ShaderObject::setFloat(const std::string name, float input)
{
	glUniform1f(glGetUniformLocation(programID, name.c_str()), input);
}

void ShaderObject::setvec2(const std::string name, glm::vec2 input)
{
	glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &input[0]);
}

void ShaderObject::setVec3(const std::string name, glm::vec3 input)
{
	glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &input[0]);
}

void ShaderObject::setMat4(const std::string name, glm::mat4 input)
{
	glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &input[0][0]);
}
