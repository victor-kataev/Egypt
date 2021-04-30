#pragma once

#include "shader.h"

struct Material
{
	Material() = default;

	Material(const std::shared_ptr<Shader>& shader, const glm::vec3 &ambient, const glm::vec3& diffuse, const glm::vec3& specular, float shininess)
	{
		Shader = shader;
		AmbientColor = ambient;
		DiffuseColor = diffuse;
		SpecularColor = specular;
		Shininess = shininess;
	}

	std::shared_ptr<Shader> GetShader()
	{
		return Shader;
	}

	std::shared_ptr<Shader> Shader;
	glm::vec3 AmbientColor;
	glm::vec3 DiffuseColor;
	glm::vec3 SpecularColor;
	float Shininess;
};