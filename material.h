#pragma once

#include "shader.h"

class Material
{
public:
	Material()
		: m_Shader("vertex.glsl", "fragment.glsl")
	{
		
	}

private:
	Shader m_Shader;
	glm::mat4 m_ProjView;
	glm::vec3 m_AmbientColor;
	glm::vec3 m_DiffuseColor;
	glm::vec3 m_SpecularColor;
	glm::vec3 m_LightColor;
};