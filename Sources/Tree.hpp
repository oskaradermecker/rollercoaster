#pragma once

#ifndef TREE_H
#define TREE_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

//Some Variables

extern std::vector<glm::vec3> TreePositions;
extern std::vector<GLfloat> TreePhi;
extern std::vector<GLfloat> TreeTheta;
extern std::vector<GLfloat> TreeSize;

//Some Functions

void generateFractalTree(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat size, GLfloat phi, GLfloat theta);
glm::mat4* CreateTreePartsModels();
#endif