#version 330 core

layout(location = 0) in vec3 vertexPosition; // Model space

uniform mat4 model;
uniform mat4 viewProjection;

void main()
{
    vec3 fragPos = vec3(model * vec4(vertexPosition, 1));
    gl_Position = viewProjection * vec4(fragPos, 1);
}
