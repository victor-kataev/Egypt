#pragma once

#include "mesh.h"

struct Material
{
	Material() = default;

	Material(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess, const std::vector<Texture>& textures = {})
	{
		AmbientColor = ambient;
		DiffuseColor = diffuse;
		SpecularColor = specular;
		Shininess = shininess;
		Textures = textures;
	}

	Material(const std::vector<Texture>& textures)
	{
		Textures = textures;
	}

	glm::vec3 AmbientColor;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;
	float Shininess;
	std::vector<Texture> Textures;
};