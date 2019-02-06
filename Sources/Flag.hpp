#pragma once

#ifndef FLAG_H
#define FLAG_H

#include <glad/glad.h>
#include <glm/glm.hpp>

//Some Functions

glm::vec4* createFlagVertices(int size_flag);
glm::vec4* createFlagVelocities(int size_flag);
glm::vec2* createFlagUV(int size_flag);
GLuint* createFlagIndices(GLuint size);
GLuint* CreateFlagBuffers(glm::vec4* vertices, glm::vec4* velocities, GLuint totalParticles);
GLuint* CreateFlagVAO(glm::vec2* uv, GLuint* indices, GLuint BufferPos, GLuint size_flag);

#endif