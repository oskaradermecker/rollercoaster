#version 330 core
layout (location = 0) in vec3 aposition; // The position variable with attribute position 0
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


out vec3 position;
out vec3 n;
out vec2 texCoord;

void main() {
	gl_Position =  projection * view * model * vec4(aposition,1.0);
	position = vec3(model*vec4(aposition,1.0));
	mat3 normalMatrix = transpose(inverse(mat3(model)));
	n = -normalize(normalMatrix * aNormal);
	texCoord = vec2(aTexCoords.x, 1-aTexCoords.y);
}