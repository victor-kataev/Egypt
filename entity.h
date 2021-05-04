#pragma once

#include "model.h"
#include "material.h"

class Entity
{
public:
	Entity(Model* model, Material material, const std::string & shaderType, const glm::vec3& pos, const glm::vec3 & scale, float angle = 0.0, glm::vec3 axis = glm::vec3(0.0, 1.0, 0.0))
		: m_Model(model), m_WorldPos(pos), m_Material(material), m_ShaderType(shaderType), m_Scale(scale)
	{
		m_ModelMatrix = glm::mat4(1.0);
		m_ModelMatrix = glm::translate(m_ModelMatrix, m_WorldPos);
		m_ModelMatrix = glm::rotate(m_ModelMatrix, glm::radians(angle), axis);
		m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
	}

	glm::vec3 GetPosition() const
	{
		return m_WorldPos;
	}

	glm::mat4 GetModelMatrix() const
	{
		return m_ModelMatrix;
	}

	void Draw(const Shader& shader) const
	{
		m_Model->DrawElements(shader);
	}

	Material& GetMaterial()
	{
		return m_Material;
	}
	
	const std::string& GetShaderType() const
	{
		return m_ShaderType;
	}

	void Rotate(float angle, glm::vec3 axis)
	{
		m_ModelMatrix = glm::mat4(1.0);
		m_ModelMatrix = glm::translate(m_ModelMatrix, m_WorldPos);
		m_ModelMatrix = glm::rotate(m_ModelMatrix, angle, axis);
		m_ModelMatrix = glm::scale(m_ModelMatrix, m_Scale);
	}

	void SetPosition(glm::vec3 pos)
	{
		m_WorldPos = pos;
	}

private:
	Model* m_Model;
	Material m_Material;
	glm::vec3 m_WorldPos;
	glm::vec3 m_Scale;
	std::string m_ShaderType;
	glm::mat4 m_ModelMatrix;
};