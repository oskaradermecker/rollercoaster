#version 330 core

out vec4 Fragcolor;
in vec3 position;
in vec3 n;

uniform vec3 camPosition ;
uniform vec3 myLight;
uniform vec3 LightCamera;
uniform samplerCube skybox;
uniform float LightPowerCamera;

void main(){

	vec3 LightColor = vec3(1.0,1.0,1.0);
	float LightPower = 100.0f;
	
	vec3 MaterialDiffuseColor = vec3(1,0.0,0.0);
	vec3 MaterialAmbientColor = MaterialDiffuseColor*0.15;
	vec3 MaterialSpecularColor = MaterialDiffuseColor *0.7;
	

	vec3 light_direction = myLight - position;
	float distance = length(light_direction);
	vec3 l = normalize(light_direction);
	float cosTheta = max(dot(n, l), 0.0);
	
	vec3 Eye = camPosition - position;
	vec3 E = normalize(Eye);
	float dist_eye = length(Eye);
	vec3 R = reflect(-l, n);  
	float cosAlpha = max(dot(E, R), 0.0);
		
	//Reflection Skybox
	vec3 Refl_vec = (2*dot(E,n)*n - E);
	vec3 ReflectionSkybox = texture(skybox, Refl_vec).rgb;

	
	//LIGHTCAMERA

	vec3 LightColorCamera = vec3(1.0,1.0,0.8);
	light_direction = LightCamera - position;
	float distance_camera = length(light_direction);
	l = normalize(light_direction);
	float cosThetaCamera = max(dot(n, l), 0.0);

	Fragcolor = 
		vec4(MaterialAmbientColor+
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance)+
		 LightColorCamera * LightPowerCamera * cosThetaCamera / pow(distance_camera,2) +
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance) +
		ReflectionSkybox, 1.0);
	}