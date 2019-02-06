#include "Tree.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

std::vector<glm::vec3> TreePositions;
std::vector<GLfloat> TreePhi;
std::vector<GLfloat> TreeTheta;
std::vector<GLfloat> TreeSize;


void generateFractalTree(GLfloat xPos, GLfloat yPos, GLfloat zPos, GLfloat size, GLfloat phi, GLfloat theta) {
	if (size < 1.5f)
		return;

	static const int maxAngle = 55;
	GLfloat newPosX = xPos + size * sin(phi / 180.0f * 3.145) *cos(theta / 180.0f * 3.145);
	GLfloat newPosZ = zPos + size * sin(phi / 180.0f * 3.145) * sin(theta / 180.0f * 3.145);
	GLfloat newPosY = yPos + size * cos(phi / 180.0f * 3.145);

	TreePositions.push_back(glm::vec3(xPos, yPos, zPos));
	TreePhi.push_back(phi);
	TreeTheta.push_back(theta);
	TreeSize.push_back(size);

	int branches = 6;
	for (int i = 0; i < branches; i++) {
		int x = rand() % 2 == 1 ? -1 : 1;
		int y = rand() % 2 == 1 ? -1 : 1;
		generateFractalTree(newPosX, newPosY, newPosZ, size*0.6f, phi + x * rand() % maxAngle, phi + y * float(rand() % 180));
	}
}


glm::mat4* CreateTreePartsModels()
{
	int amount = TreePhi.size();
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];

	glm::mat4 model1 = glm::scale(glm::mat4(1.0), glm::vec3(TreeSize[0] * 0.02, TreeSize[0] * 0.505, TreeSize[0] * 0.02));
	glm::mat4 model2 = glm::rotate(glm::mat4(1.0), -TreePhi[0] / 180.0f*3.14f, glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 model3 = glm::rotate(glm::mat4(1.0), -TreeTheta[0] / 180.0f*3.14f, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 model4 = glm::translate(glm::mat4(1.0), TreePositions[0] + glm::vec3(-70.0, 0.2, -80.0));

	modelMatrices[0] = model4 * model3 * model2 * model1;

	for (unsigned int i = 1; i < amount; i++)
	{
		glm::mat4 model1 = glm::scale(glm::mat4(1.0), glm::vec3(TreeSize[i] * 0.03, TreeSize[i] * 0.505, TreeSize[i] * 0.03));
		glm::mat4 model2 = glm::rotate(glm::mat4(1.0), -TreePhi[i] / 180.0f*3.14f, glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 model3 = glm::rotate(glm::mat4(1.0), -TreeTheta[i] / 180.0f*3.14f, glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 model4 = glm::translate(glm::mat4(1.0), TreePositions[i] + glm::vec3(-70.0, 0.2, -80.0));


		modelMatrices[i] = model4 * model3*model2*model1;
	}
	return modelMatrices;
}

