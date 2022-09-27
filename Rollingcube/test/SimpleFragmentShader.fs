#version 330 core

in vec4 fragmentColor;

out vec3 color;

void main()
{

	// Output color = red 
	color = fragmentColor.xyz;

}