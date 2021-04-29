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

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

//uniform vec3 objectColor;
//uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewerPos;
//uniform float time;

uniform Material material;
uniform Light light;

void main()
{
	//ambient
	vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoords));

	//diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	float diffuseImpact = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diffuseImpact * vec3(texture(material.texture_diffuse1, TexCoords));

	//specular
	vec3 viewDir = normalize(viewerPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float specComponent = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * specComponent * vec3(texture(material.texture_specular1, TexCoords));

//	vec3 emission = vec3(0.0);
//	if(texture(material.texture_specular1, TexCoords).rgb == vec3(0.0))
//	{
//		emission = texture(material.emission, TexCoords + vec2(0.0, time)).rgb;
//	}

	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0);
}