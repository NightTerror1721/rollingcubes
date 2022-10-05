#version 330 core

struct MaterialInfo
{
	vec3 diffuseColor;
	vec3 ambientColor;
	vec3 specularColor;
	
	float kd;
	float ka;
	float ks;
	
	float shininess;
};

in vec2 uv;
in vec3 position; // World space
in vec3 normal; // Camera space
in vec3 eyeDirection; // Camera space
in vec3 lightDirection; //Camera space

out vec3 color;

uniform sampler2D mainTexture;
uniform vec3 lightPos; // World space

uniform MaterialInfo material;

struct LightInfo
{
	vec3 pos;
	vec3 direction;
	vec3 color;
	float power;
};

vec3 computeColorFromLight(LightInfo light);


void main()
{
	vec3 lightColor = vec3(1, 1, 1);
	float lightPower = 20.f;
	
	LightInfo light = LightInfo(
			lightPos,
			lightDirection,
			lightColor,
			lightPower
	);
	
	color = computeColorFromLight(light);
}

vec3 computeColorFromLight(LightInfo light)
{
	vec3 diffuseColor = texture(mainTexture, uv).rgb * material.diffuseColor;
	vec3 ambientColor = material.ambientColor * diffuseColor;
	vec3 specularColor;
	
	float dist = length(light.pos - position);
	
	vec3 n = normalize(normal);
	vec3 l = normalize(light.direction);
	
	float cosTheta = clamp(dot(n, l), 0, 1);
	diffuseColor *= light.color * light.power * cosTheta / (dist * dist);
	
	vec3 eye = normalize(eyeDirection);
	vec3 refl = reflect(-l, n);
	
	float cosAlpha = clamp(dot(eye, refl), 0, 1);
	
	float shininessFactor = pow(cosAlpha, material.shininess);
	specularColor = material.specularColor * light.color * light.power * shininessFactor / (dist * dist);
	
	return
			(ambientColor * material.ka) +
			(diffuseColor * material.kd) +
			(specularColor * material.ks);
}
