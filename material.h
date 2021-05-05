#pragma once

#include "mesh.h"

struct Material
{
	Material() = default;

	Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess)
	{
		AmbientColor = ambient;
		DiffuseColor = diffuse;
		SpecularColor = specular;
		Shininess = shininess;
	}

	void Bind(Shader & shader)
	{
		shader.setFloat("material.shininess", Shininess);
		shader.setVec3("material.ambient", AmbientColor);
		shader.setVec3("material.diffuse", DiffuseColor);
		shader.setVec3("material.specular", SpecularColor);
	}

	glm::vec3 AmbientColor;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;
	float Shininess;
};