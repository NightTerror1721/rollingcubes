#version 330 core

#define MAX_LIGHTS 8

struct ColorChannels
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material
{
	ColorChannels color;
	sampler2D diffuse;
	sampler2D specular;
    sampler2D normals;
	float shininess;
    float opacity;
};

struct PointLight
{
	vec3 position;
	ColorChannels color;
	float constant;
	float linear;
	float quadratic;
	float intensity;
};

struct DirectionalLight
{
	vec3 direction;
	ColorChannels color;
	float intensity;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in mat3 TBN;

out vec4 FragColor;

uniform vec3 viewPos;
uniform int pointLightsCount;
uniform bool useNormalMapping;
uniform bool useMainPointLight;
uniform DirectionalLight dirLight;
uniform PointLight mainPointLight;
uniform PointLight pointLights[MAX_LIGHTS];
uniform Material material;

vec3 computeColorFromDirectionalLight(vec3 normal, vec3 viewDir);
vec3 computeColorFromLight(PointLight light, vec3 normal, vec3 viewDir);


void main()
{
    vec3 normal;
    if(useNormalMapping)
    {
        normal = texture(material.normals, TexCoords).rgb;
        normal = normal * 2 - 1;
        normal = normalize(TBN * normal);
    }
    else
    {
        normal = normalize(Normal);
    }

	vec3 viewDir = normalize(viewPos - FragPos);

	// == =====================================================
    // Our lighting is set up in 2 phases: directional and point lights
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================

    // phase 1: directional lighting
	vec3 result = computeColorFromDirectionalLight(normal, viewDir);

	// phase 2: point lights
    if(useMainPointLight)
        result += computeColorFromLight(mainPointLight, normal, viewDir);
	int len = clamp(pointLightsCount, 0, MAX_LIGHTS);
	for(int i = 0; i < len; ++i)
		result += computeColorFromLight(pointLights[i], normal, viewDir);

	FragColor = vec4(result, clamp(material.opacity, 0, 1));
}

vec3 computeColorFromDirectionalLight(vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirLight.direction);

    // Diffuse Shading //
    float diffFactor = max(dot(normal, lightDir), 0.0);

    // Specular Shading //
    vec3 reflectDir = reflect(-lightDir, normal);
    float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Combine results //
    vec3 ambient = dirLight.color.ambient * vec3(texture(material.diffuse, TexCoords)) * material.color.ambient;
    vec3 diffuse = dirLight.color.diffuse * dirLight.intensity * diffFactor * vec3(texture(material.diffuse, TexCoords)) * material.color.diffuse;
    vec3 specular = dirLight.color.specular * dirLight.intensity * specFactor * vec3(texture(material.specular, TexCoords)) * material.color.specular;

    return (ambient + diffuse + specular);
}

vec3 computeColorFromLight(PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);

    // Diffuse Shading //
    float diffFactor = max(dot(normal, lightDir), 0.0);

    // Specular Shading //
    vec3 reflectDir = reflect(-lightDir, normal);
    float specFactor = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // Attenuation //
    float dist = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));   
	
    // Combine results //
    vec3 ambient = light.color.ambient * vec3(texture(material.diffuse, TexCoords)) * material.color.ambient;
    vec3 diffuse = light.color.diffuse * light.intensity * diffFactor * vec3(texture(material.diffuse, TexCoords)) * material.color.diffuse;
    vec3 specular = light.color.specular * light.intensity * specFactor * vec3(texture(material.specular, TexCoords)) * material.color.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}
