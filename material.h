#pragma once

#include "mesh.h"

struct Material
{
	Material() = default;
	Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
	void Bind(Shader& shader);

	glm::vec3 AmbientColor;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;
	float Shininess;
};