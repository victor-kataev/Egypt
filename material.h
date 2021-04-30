#pragma once

struct Material
{
	Material() = default;

	Material(const glm::vec3 &ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, const glm::vec3 & baseColor)
	{
		AmbientColor = ambient;
		DiffuseColor = diffuse;
		SpecularColor = specular;
		Shininess = shininess;
	}

	glm::vec3 AmbientColor;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;
	float Shininess;
	glm::vec3 BaseColor;
};