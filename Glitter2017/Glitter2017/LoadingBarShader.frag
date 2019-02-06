#version 330 core

in vec2 TexCoords;

out vec4 outcolor;

uniform sampler2D loadingTexture;

void main(){

	outcolor = texture(loadingTexture, TexCoords);  ;
		
}