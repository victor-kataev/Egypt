#pragma once

#include "shader.h"

class Material
{
public:

	Material() = default;

	Material(const std::shared_ptr<Shader>& shader, const glm::vec3 &ambient, const glm::vec3& diffuse, const glm::vec3& specular)
	{
		m_Shader = shader;
		m_AmbientColor = ambient;
		m_DiffuseColor = diffuse;
		m_SpecularColor = specular;
	}

private:
	std::shared_ptr<Shader> m_Shader;
	glm::vec3 m_AmbientColor;
	glm::vec3 m_DiffuseColor;
	glm::vec3 m_SpecularColor;
};