#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 4) in mat4 aInstanceMatrix;

uniform mat4 lightSpaceMatrix;


void main()
{
	mat4 model = aInstanceMatrix;
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0); // 
}