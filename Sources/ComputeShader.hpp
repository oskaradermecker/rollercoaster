#pragma once

#ifndef COMPUTESHADER_H
#define COMPUTESHADER_H

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class ComputeShader {
public:
	// State
	GLuint ID;

	// Constructor
	ComputeShader(const GLchar *computeSource);

	ComputeShader& use();

	void compile();

	void setFloat(const GLchar *name, GLfloat value);
	void setInteger(const GLchar *name, GLint value);

	void setVector2f(const GLchar *name, GLfloat x, GLfloat y);
	void setVector2f(const GLchar *name, const glm::vec2 &value);

	void setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
	void setVector3f(const GLchar *name, const glm::vec3 &value);

	void setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void setVector4f(const GLchar *name, const glm::vec4 &value);

	void setMatrix4(const GLchar *name, const glm::mat4 &matrix);

private:
	// Checks if compilation or linking failed and if so, print the error logs
	void checkCompileErrors(const GLuint &object, std::string type);

	// Make a shader from a filepath
	GLuint pathToShader(const GLchar * path, GLenum shaderType);

	// Get a string of the shader type from the GLenum
	std::string shaderTypeToString(GLenum shaderType);

	const GLchar* mComputePath;

};

#endif