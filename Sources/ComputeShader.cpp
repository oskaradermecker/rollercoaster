#include "ComputeShader.hpp"

#include <fstream>
#include <sstream>

#include <iostream>

ComputeShader::ComputeShader(const GLchar *computePath) {
	mComputePath = computePath;
}

ComputeShader &ComputeShader::use() {
	glUseProgram(ID);
	return *this;
}

void ComputeShader::compile() {

	GLuint computeShader = pathToShader(mComputePath, GL_COMPUTE_SHADER);


	// Create Program
	ID = glCreateProgram();
	
	// Attach the shaders to the program
	glAttachShader(ID, computeShader);


	glLinkProgram(ID);
	checkCompileErrors(ID, "Program");

	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(computeShader);

}

void ComputeShader::setFloat(const GLchar *name, GLfloat value) {
	glUniform1f(glGetUniformLocation(ID, name), value);
}
void ComputeShader::setInteger(const GLchar *name, GLint value) {
	glUniform1i(glGetUniformLocation(ID, name), value);
}
void ComputeShader::setVector2f(const GLchar *name, GLfloat x, GLfloat y) {
	glUniform2f(glGetUniformLocation(ID, name), x, y);
}
void ComputeShader::setVector2f(const GLchar *name, const glm::vec2 &value) {
	glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}
void ComputeShader::setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z) {
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}
void ComputeShader::setVector3f(const GLchar *name, const glm::vec3 &value) {
	glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}
void ComputeShader::setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}
void ComputeShader::setVector4f(const GLchar *name, const glm::vec4 &value) {
	glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
}
void ComputeShader::setMatrix4(const GLchar *name, const glm::mat4 &matrix) {
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void ComputeShader::checkCompileErrors(const GLuint &object, std::string type) {
	GLint success;
	GLchar infoLog[1024];
	
	if (type != "Program") {
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " << type << std::endl;
			std::cout << infoLog << "\n -- --------------------------------------------------- -- \n";
		}
	} else {
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " << type << std::endl;
			std::cout << infoLog << "\n -- --------------------------------------------------- -- \n";
		}
	}
}

GLuint ComputeShader::pathToShader(const GLchar* path, GLenum shaderType) {
	std::ifstream shaderFile;
	std::stringstream shaderStream;

	shaderFile.open(path);
	shaderStream << shaderFile.rdbuf();
	shaderFile.close();

	std::string code = shaderStream.str();
	const GLchar * ccode = code.c_str();


	GLuint shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &ccode, NULL);
	glCompileShader(shader);
	checkCompileErrors(shader, shaderTypeToString(shaderType));

	return shader;
}

std::string ComputeShader::shaderTypeToString(GLenum shaderType) {
	std::string ret = "";

	switch (shaderType) {
	case GL_COMPUTE_SHADER:
		ret = "Compute";
		break;
	default:
		ret = "unknown";
		break;
	}
	return ret;
}
