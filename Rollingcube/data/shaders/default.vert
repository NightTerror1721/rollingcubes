#version 330 core

layout(location = 0) in vec3 vertexPosition; // Model space
layout(location = 1) in vec2 vertexUV; // Model space
layout(location = 2) in vec3 vertexNormal; // Model space

uniform mat4 model;
uniform mat4 viewProjection;
uniform mat3 modelNormal;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * vec4(vertexPosition, 1));
    Normal = modelNormal * vertexNormal;
    TexCoords = vertexUV;

    gl_Position = viewProjection * vec4(FragPos, 1);
}
