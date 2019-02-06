#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float height;
in vec3 position;
in float random;


uniform sampler2D texture;

void main()
{
    FragColor = vec4(1.0,height/4.5,0.0,1.0-height*0.1-random*0.5);
}