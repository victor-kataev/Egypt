#pragma once

#include "model.h"
#include "material.h"

class Entity
{
public:
	Entity(Model* model, Material material, const std::string & shaderType, const glm::vec3& pos, const glm::vec3 & scale)
		: m_Model(model), m_WorldPos(pos), m_Material(material), m_ShaderType(shaderType), m_Scale(scale)
	{
		m_ModelMatrix = glm::mat4(1.0);
		m_ModelMatrix = glm::translate(m_ModelMatrix, m_WorldPos);
		m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
		
	}


	glm::mat4 GetModelMatrix() const
	{
		return m_ModelMatrix;
	}

	void Draw(const Shader& shader) const
	{
		m_Model->Draw(shader);
	}

	Material& GetMaterial()
	{
		return m_Material;
	}
	
	const std::string& GetShaderType() const
	{
		return m_ShaderType;
	}

private:
	Model* m_Model;
	Material m_Material;
	glm::vec3 m_WorldPos;
	glm::vec3 m_Scale;
	std::string m_ShaderType;
	glm::mat4 m_ModelMatrix;
};