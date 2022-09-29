#version 330 core

in vec2 uv;
in vec3 position; // World space
in vec3 normal; // Camera space
in vec3 eyeDirection; // Camera space
in vec3 lightDirection; //Camera space

out vec3 color;

uniform sampler2D mainTexture;
uniform vec3 lightPos; // World space

uniform vec3 materialDiffuseColor;
uniform vec3 materialAmbientColor;
uniform vec3 materialSpecularColor;

uniform float materialDiffuseK;
uniform float materialAmbientK;
uniform float materialSpecularK;

uniform float materialShininess;

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
	float lightPower = 50.f;
	
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
	vec3 diffuseColor = texture(mainTexture, uv).rgb * materialDiffuseColor;
	vec3 ambientColor = materialAmbientColor * diffuseColor;
	vec3 specularColor;
	
	float dist = length(light.pos - position);
	
	vec3 n = normalize(normal);
	vec3 l = normalize(light.direction);
	
	float cosTheta = clamp(dot(n, l), 0, 1);
	diffuseColor *= light.color * light.power * cosTheta / (dist * dist);
	
	vec3 eye = normalize(eyeDirection);
	vec3 refl = reflect(-l, n);
	
	float cosAlpha = clamp(dot(eye, refl), 0, 1);
	
	float shininessFactor = pow(cosAlpha, materialShininess);
	specularColor = materialSpecularColor * light.color * light.power * shininessFactor / (dist * dist);
	
	return
			(ambientColor * materialAmbientK) +
			(diffuseColor * materialDiffuseK) +
			(specularColor * materialSpecularK);
}
