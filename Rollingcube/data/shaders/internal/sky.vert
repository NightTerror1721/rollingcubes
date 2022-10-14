#version 330 core

layout(location = 0) in vec3 vertexPosition;

out vec3 TexCoord;

uniform mat4 view;
uniform mat4 projection;


void main()
{
	TexCoord = vertexPosition;
	
	gl_Position = projection * view * vec4(vertexPosition, 1);
}
