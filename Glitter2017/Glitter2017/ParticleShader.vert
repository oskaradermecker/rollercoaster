#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 4) in mat4 aInstanceMatrix;
layout (location = 8) in float RandomNumber;

out vec2 TexCoords;
out float height;
out vec3 position;
out float random;

uniform mat4 projection;
uniform mat4 view;
uniform float time;
uniform vec3 translation;

void main()
{
    TexCoords = aTexCoords;
	mat4 model = aInstanceMatrix;
	float distance = length(vec3(model[3][0],0.0,model[3][2]));
	float max_height= RandomNumber*4 +1.0;
	model[3][1]+=3*time;
	model[3][1]=float(model[3][1]-(max_height*floor(model[3][1]/max_height)));
	height = model[3][1];
	model[3][0]-=model[3][0]/(distance)*(height/4.5+RandomNumber/1.8);
	model[3][2]-=model[3][2]/(distance)*(height/4.5+RandomNumber/1.8);
	
	model[3][0]+=translation.x;
	model[3][1]+=translation.y;
	model[3][2]+=translation.z;

	position = aPos;
    gl_Position = projection * view * model* vec4(position,1.0);  
	random = RandomNumber;
}