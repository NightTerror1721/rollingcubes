#version 330 core

uniform mat4 model;
uniform mat4 projection;

layout(location = 0) in vec2 vertexPosition; // Model space
layout(location = 1) in vec2 vertexTexCoord; // Model space

smooth out vec2 ioVertexTexCoord;

void main()
{
	mat4 mvp = projection * model;
	gl_Position = mvp * vec4(vertexPosition, 0.0, 1.0);
	ioVertexTexCoord = vertexTexCoord;
}
