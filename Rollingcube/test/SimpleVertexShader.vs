#version 330 core

layout(location = 0) in vec3 vertexPosition; // Camera space
layout(location = 1) in vec2 vertexUV; // Camera space
layout(location = 2) in vec3 vertexNormal; // Camera space

uniform mat4 viewProjection;
uniform mat4 model;

out vec2 uv;

void main()
{
	mat4 mvp = model * viewProjection;
    gl_Position = mvp * vec4(vertexPosition, 1);

    uv = vertexUV;
}
