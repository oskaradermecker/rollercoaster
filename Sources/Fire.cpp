#include "Fire.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <vector>

glm::mat4* CreateParticleModels(unsigned int amount, float radius, float offset)
{
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];

	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0);

		// 1. translation
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 6 * 100)) / 100.0f - offset * 6;
		float y = displacement * 6;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		//2. Scaling
		float scale = (rand() % 20) / 50.0f + 0.005f;
		model = glm::scale(model, glm::vec3(scale));

		//3. Rotation
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		modelMatrices[i] = model;

	}
	return modelMatrices;

}



