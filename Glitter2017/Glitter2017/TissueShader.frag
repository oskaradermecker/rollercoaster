#version 330 core

in vec3 position;
out vec4 Fragcolor;
in vec2 texCoord;

uniform sampler2D Texture;

void main(){
	Fragcolor = texture(Texture,texCoord);
	}