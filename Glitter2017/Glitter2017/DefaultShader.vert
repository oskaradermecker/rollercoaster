#version 330 core
layout (location = 0) in vec3 aposition; // The position variable with attribute position 0
layout (location = 1) in vec3 aNormal; // The normal variable with attribute position 1
layout (location = 2) in vec2 itexCoord; // The itexCoord variable with attribute position 2
layout (location = 3) in vec3 aTangent; // The itexCoord variable with attribute position 2

out vec3 position;
out vec2 texCoord; 
out vec4 FragPosLightSpace;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 biaslightSpaceMatrix;
uniform mat4 model;
uniform vec3 myLight;
uniform vec3 camPosition ;



void main() {

	gl_Position =  projection * view * model * vec4(aposition, 1.0); 
	position = vec3(model * vec4(aposition, 1.0)); //
	texCoord = vec2(1.0- itexCoord.x, itexCoord.y); //
	FragPosLightSpace = biaslightSpaceMatrix * vec4(position, 1.0);

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    T = normalize(T - dot(T, N) * N); //Gram-Schmidt
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T,B,N));    
    TangentLightPos = TBN * myLight;
    TangentViewPos  = TBN * camPosition ;
    TangentFragPos  = TBN * position;


}
