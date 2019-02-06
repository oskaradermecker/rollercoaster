#version 330 core

in vec3 position;
in vec2 texCoord; 
in vec4 FragPosLightSpace;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

out vec4 Fragcolor;


// Values that stay constant for the whole mesh.
uniform sampler2D myTexture;
uniform sampler2D NormalMap;
uniform sampler2D depth;

void main(){

    vec3 normal = texture(NormalMap, texCoord).rgb;
    vec3 n = normalize(normal * 2.0 - 1.0); 

	vec3 LightColor = vec3(1.0,1.0,1.0);
	float LightPower = 250.0f;
	
	vec3 MaterialDiffuseColor = texture(myTexture, texCoord ).rgb;
	vec3 MaterialAmbientColor = vec3(0.15,0.15,0.15) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = MaterialDiffuseColor *0.3;

	//Diffuse
	vec3 light_direction = TangentLightPos - TangentFragPos;
	float distance = length(light_direction);
	vec3 l = normalize(light_direction);
	float cosTheta = max(dot(n, l), 0.0);
	

	//Specular	
	vec3 Eye = TangentViewPos - TangentFragPos;
	vec3 E = normalize(Eye);
	float dist_eye = length(Eye);
	vec3 R = reflect(-l, n);  
	float cosAlpha = max(dot(E, R), 0.0);
	
	//Shadow
	vec2 poissonDisk[4] = vec2[](
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 ));

	float bias = 0.002*tan(acos(cosTheta));
	bias = clamp(bias, 0,0.01);
	float visibility = 1.0; 
	for (int i=0;i<4;i++){
		if (texture(depth, FragPosLightSpace.xy+ poissonDisk[i]/500.0).r +bias  < FragPosLightSpace.z){ 
		visibility -= 0.25; 
	}   
}

	Fragcolor = 
		vec4(MaterialAmbientColor+
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance)*visibility+
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance) *(visibility) ,1.0) ;
		
}