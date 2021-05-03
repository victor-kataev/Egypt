#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 color;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D emission;
	float shininess;
};

struct Light
{
	vec3 position;
	vec3 dir;
	float cutoff;
	float outerCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight
{
	vec3 dir;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct SpotlightLight
{
	vec3 position;
	vec3 dir;

	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


//uniform vec3 lightPos;
uniform vec3 viewerPos;
//uniform float time;
uniform vec3 SunColor;
uniform vec3 SunPosition;

uniform Material material;
//uniform Light light;
#define PLN 10
uniform int nPointLights;

uniform DirectionalLight dirLight;
uniform PointLight pointLight[PLN];
uniform SpotlightLight spotlight;


vec3 DirLightImpact(vec3 normal);
vec3 PointLightImpact(PointLight pointLight, vec3 normal);
vec3 SpotlightImpact(vec3 normal);

void main()
{
/*
	vec3 lightDir = normalize(light.position - FragPos);
	float theta = dot(lightDir, normalize(-light.dir));
	float epsilon = light.cutoff - light.outerCutoff;
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);
	vec3 result;

	//ambient
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

	
	//diffuse
	vec3 norm = normalize(Normal);
	//vec3 lightDir = normalize(-light.dir);
	float diffuseImpact = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diffuseImpact * vec3(texture(material.texture_diffuse1, TexCoords));

	//specular
	vec3 viewDir = normalize(viewerPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specComponent = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * specComponent * vec3(texture(material.texture_specular1, TexCoords));

	float d = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * (d*d));
	
	diffuse*= intensity;
	specular *= intensity;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	result = ambient + diffuse + specular;
	*/

	vec3 result = vec3(0.0);
	vec3 norm = normalize(Normal);
	
	result += DirLightImpact(norm);
	for(int i = 0; i < nPointLights; i++)
		result += PointLightImpact(pointLight[i], norm);
	//result += SpotlightImpact(norm);


//	vec3 emission = vec3(0.0);
//	if(texture(material.texture_specular1, TexCoords).rgb == vec3(0.0))
//	{
//		emission = texture(material.emission, TexCoords + vec2(0.0, time)).rgb;
//	}

	

	color = vec4(result, 1.0);
}


vec3 DirLightImpact(vec3 normal)
{
	vec3 lightDir = normalize(-dirLight.dir);
	
	float diffuseImpact = max(dot(lightDir, normal), 0.0);

	vec3 reflected = reflect(-lightDir, normal);
	vec3 viewerDir = normalize(viewerPos - FragPos);
	float specularImpact = pow(max(dot(reflected, viewerDir), 0.0), material.shininess);

	vec3 ambient = dirLight.ambient * SunColor * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 diffuse = dirLight.diffuse * diffuseImpact * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 specular = dirLight.specular * specularImpact * vec3(texture(material.texture_specular1, TexCoords));

	return ambient + diffuse + specular;
}

vec3 PointLightImpact(PointLight light, vec3 normal)
{
	vec3 lightDir = normalize(light.position - FragPos);

	float diffuseImpact = max(dot(lightDir, normal), 0.0);

	vec3 reflected = reflect(-lightDir, normal);
	vec3 viewerDir = normalize(viewerPos - FragPos);
	float specularImpact = pow(max(dot(reflected, viewerDir), 0.0) , material.shininess);

	float d = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * d + light.quadratic * (d*d));

	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 diffuse = light.diffuse * diffuseImpact * vec3(texture(material.texture_diffuse1, TexCoords));
	vec3 specular = light.specular * specularImpact * vec3(texture(material.texture_specular1, TexCoords));

	ambient *= attenuation;
	diffuse*= attenuation;
	specular *= attenuation;

	return ambient + diffuse + specular;
}

vec3 SpotlightImpact(vec3 normal)
{
	vec3 lightDir = normalize(spotlight.position - FragPos);
	float theta = dot(lightDir, normalize(-spotlight.dir));
	float epsilon = spotlight.cutOff - spotlight.outerCutOff;
	float intensity = clamp((theta - spotlight.outerCutOff) / epsilon, 0.0, 1.0);


	PointLight pl;
	pl.position = spotlight.position;
	pl.constant = spotlight.constant;
	pl.linear = spotlight.linear;
	pl.quadratic = spotlight.quadratic;
	pl.ambient = spotlight.ambient;
	pl.diffuse = spotlight.diffuse;
	pl.specular = spotlight.specular;
	vec3 result = PointLightImpact(pl, normal);
	return result * intensity;
}
