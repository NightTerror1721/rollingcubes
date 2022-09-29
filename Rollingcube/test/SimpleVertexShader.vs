#version 330 core

layout(location = 0) in vec3 vertexPosition; // Model space
layout(location = 1) in vec2 vertexUV; // Model space
layout(location = 2) in vec3 vertexNormal; // Model space

uniform mat4 viewProjection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 lightPos; // World space

out vec2 uv;
out vec3 position; // World space
out vec3 normal; // Camera space
out vec3 eyeDirection; // Camera space
out vec3 lightDirection; //Camera space

void main()
{
	mat4 mvp = model * viewProjection;
    gl_Position = mvp * vec4(vertexPosition, 1);
    
    //vertexPos: Camera space
    vec3 vertexPos = (view * model * vec4(vertexPosition, 1)).xyz;
    eyeDirection = vec3(0, 0, 0) - vertexPos;
    
    //lightPosCS: Camera space
    vec3 lightPosCS = (view * vec4(lightPos, 1)).xyz;
    lightDirection = lightPosCS + eyeDirection;
    
    normal = (view * model * vec4(vertexNormal, 0)).xyz;
    
    

    uv = vertexUV;
}
