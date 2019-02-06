#version 330 core
layout (location = 0) in vec4 aposition; // The position variable with attribute position 0
layout (location = 2) in vec2 aTexCoords;

out vec3 position;
out vec2 texCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;


void main() {
	gl_Position =  projection * view * model* aposition; 
	position = vec3(model*aposition);
	texCoord = vec2(aTexCoords.x, 1-aTexCoords.y);
}