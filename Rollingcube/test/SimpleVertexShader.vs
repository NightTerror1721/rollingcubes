#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 5) in vec4 vertexColor;

out vec4 fragmentColor;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(vertexPosition_modelspace, 1);

    fragmentColor = vertexColor;
}
