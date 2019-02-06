#include "Flag.hpp"


glm::vec4* createFlagVertices(int size_flag)
{
	int totalParticles = size_flag * size_flag;
	glm::vec4* vertices;
	vertices = new glm::vec4[totalParticles];

	for (int i = 0; i < totalParticles; i += 1) {
		vertices[i] = glm::vec4(0.0f, floor(i / size_flag)*10.0 / size_flag * 1 + 40.0f, float(i % int(size_flag))*14.0 / size_flag, 1.0f);
	}
	return vertices;
}

glm::vec4* createFlagVelocities(int size_flag)
{
	int totalParticles = size_flag * size_flag;
	glm::vec4* velocities;
	velocities = new glm::vec4[totalParticles];

	for (int i = 0; i < totalParticles; i += 1) {
		velocities[i] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	}
	return velocities;
}

glm::vec2* createFlagUV(int size_flag)
{
	int totalParticles = size_flag * size_flag;
	glm::vec2* uv;
	uv = new glm::vec2[totalParticles];

	for (int i = 0; i < totalParticles; i += 1) {
		uv[i] = glm::vec2(float(i % int(size_flag)) / size_flag, floor(i / size_flag) / size_flag);
	}
	return uv;
}

GLuint* createFlagIndices(GLuint size)
{
	GLuint* indices;
	indices = new GLuint[(size - 1)*(size - 1) * 2 * 3];

	int j = 0;

	for (int i = 0; i < size*size; i += 1) {

		int x = i % size;
		int y = floor(i / size);

		if (x < size - 1 & y < size - 1) {
			indices[6 * j] = i;
			indices[6 * j + 1] = i + 1;
			indices[6 * j + 2] = i + 1 + size;
			indices[6 * j + 3] = i;
			indices[6 * j + 4] = i + size;
			indices[6 * j + 5] = i + 1 + size;
			j += 1;

		}
	}
	return indices;
}

GLuint* CreateFlagBuffers(glm::vec4* vertices, glm::vec4* velocities, GLuint totalParticles)
{
	GLuint* Buffers = new GLuint[4];
	GLuint bufSize = totalParticles * 4 * sizeof(GLfloat);

	GLuint posBufIn;
	glGenBuffers(1, &posBufIn);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, posBufIn);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &vertices[0],
		GL_DYNAMIC_DRAW);
	Buffers[0] = posBufIn;

	GLuint posBufOut;
	glGenBuffers(1, &posBufOut);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, posBufOut);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &vertices[0],
		GL_DYNAMIC_DRAW);
	Buffers[1] = posBufOut;

	GLuint velBufIn;
	glGenBuffers(1, &velBufIn);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, velBufIn);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &velocities[0],
		GL_DYNAMIC_DRAW);
	Buffers[2] = velBufIn;

	GLuint velBufOut;
	glGenBuffers(1, &velBufOut);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, velBufOut);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufSize, &velocities[0],
		GL_DYNAMIC_DRAW);
	Buffers[3] = velBufOut;

	return Buffers;
}

GLuint* CreateFlagVAO(glm::vec2* uv, GLuint* indices, GLuint BufferPos, GLuint size_flag)
{

	GLuint* VAO_EBO = new GLuint[2];

	GLuint totalParticles = size_flag * size_flag;
	GLuint bufSizeUV = totalParticles * 2 * sizeof(GLfloat);

	GLuint VAO_tissue, VBO_tissue_uv, EBO_tissue;
	glGenVertexArrays(1, &VAO_tissue);
	glGenBuffers(1, &EBO_tissue);
	VAO_EBO[0] = VAO_tissue;
	VAO_EBO[1] = EBO_tissue;


	glBindVertexArray(VAO_tissue);

	glBindBuffer(GL_ARRAY_BUFFER, BufferPos);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);


	glGenBuffers(1, &VBO_tissue_uv);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_tissue_uv);
	glBufferData(GL_ARRAY_BUFFER, bufSizeUV, &uv[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_tissue);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (size_flag - 1)*(size_flag - 1) * 2 * 3 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	return VAO_EBO;

}