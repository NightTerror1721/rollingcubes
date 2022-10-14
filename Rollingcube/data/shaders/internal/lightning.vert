#version 330 core

layout(location = 0) in vec3 vertexPosition; // Model space
layout(location = 1) in vec2 vertexUV; // Model space
layout(location = 2) in vec3 vertexNormal; // Model space
layout(location = 3) in vec3 vertexTangent; // Model space
layout(location = 4) in vec3 vertexBitangent; // Model space

uniform mat4 model;
uniform mat4 viewProjection;
uniform mat3 modelNormal;
uniform bool useNormalMapping;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
    FragPos = vec3(model * vec4(vertexPosition, 1));
    Normal = modelNormal * vertexNormal;
    TexCoords = vertexUV;

    if(useNormalMapping)
    {
        vec3 T = normalize(vec3(model * vec4(vertexTangent, 0)));
        vec3 B = normalize(vec3(model * vec4(vertexBitangent, 0)));
        vec3 N = normalize(vec3(model * vec4(vertexNormal, 0)));
        TBN = mat3(T, B, N);
    }

    gl_Position = viewProjection * vec4(FragPos, 1);
}
