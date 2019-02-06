#version 330 core

out vec4 Fragcolor;
in vec3 position;
in vec3 n;

uniform vec3 camPosition;
uniform vec3 myLight;

void main(){

	vec3 LightColor = vec3(1.0,1.0,1.0);
	float LightPower = 85.0f;
	
	vec3 MaterialDiffuseColor = vec3((position.y-30)/20,(position.y-30)/20,(position.y-30)/20);
	vec3 MaterialAmbientColor = MaterialDiffuseColor * 0.15;
	vec3 MaterialSpecularColor = MaterialDiffuseColor * 0.3;
	

	vec3 light_direction = myLight - position;
	float distance = length(light_direction);
	vec3 l = normalize(light_direction);
	float cosTheta = max(dot(n, l), 0.0);
	
	vec3 Eye = camPosition - position;
	vec3 E = normalize(Eye);
	float dist_eye = length(Eye);
	vec3 R = reflect(-l, n);  
	float cosAlpha = max(dot(E, R), 0.0);
	
	Fragcolor = vec4 (
	MaterialAmbientColor +
	MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance) +
	MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance), 1.0);
}