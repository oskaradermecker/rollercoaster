#version 330 core
layout (location = 0) in vec3 position; // The position variable with attribute position 0
layout (location = 1) in vec2 itexCoord; // The itexCoord variable with attribute position 1

out vec2 TexCoords;

void main() {

	gl_Position =  vec4(position, 1.0); 
	TexCoords = itexCoord;

}
